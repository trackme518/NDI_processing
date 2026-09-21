// Builds the NDI_p5 JNI shared libraries for the host platform using the host C++ compiler,
// and packages them (plus the NDI runtime, when available) into a jar that the root project
// bundles inside the Processing library jar.
//
// Natives inside the jar follow the layout expected by NDIStream.java:
//   natives/<os>/<arch>/<library>   e.g.  natives/macos/arm64-v8a/libndistream-natives.dylib

import org.gradle.internal.os.OperatingSystem
import java.nio.file.Files
import java.nio.file.Path

plugins {
    base
    `java-base`
}

val javaToolchains = project.extensions.getByType(JavaToolchainService::class)
val jniCompiler = javaToolchains.compilerFor {
    languageVersion.set(JavaLanguageVersion.of(17))
}

// JNI headers generated from the Java sources in the root project
val headerOnly: Configuration by configurations.creating {
    isCanBeResolved = true
    isCanBeConsumed = false
}
dependencies {
    headerOnly(project(":", "jniIncludes"))
}

// gulrak/filesystem, used as a drop-in for C++17 <filesystem> on older toolchains
val filesystemHeaderDir = layout.buildDirectory.dir("deps")
val downloadFilesystemHeader by tasks.registering {
    val dest = filesystemHeaderDir.map { it.file("filesystem.hpp") }
    outputs.file(dest)

    doLast {
        val target = dest.get().asFile
        if (!target.exists()) {
            target.parentFile.mkdirs()
            uri("https://github.com/gulrak/filesystem/releases/download/v1.3.2/filesystem.hpp")
                .toURL().openStream().use { input -> target.outputStream().use { input.copyTo(it) } }
        }
    }
}

fun locateNdiSdk(): Path? {
    // Check system property
    var ndiSdk = if (System.getProperty("ndiSdk") != null) file(System.getProperty("ndiSdk")).toPath() else null

    // Check "NDI_SDK_DIR" environment variable
    if (ndiSdk == null && System.getenv("NDI_SDK_DIR") != null) {
        ndiSdk = file(System.getenv("NDI_SDK_DIR")).toPath()
    }

    // Check typical install locations
    if (ndiSdk == null && OperatingSystem.current().isWindows && file("C:/Program Files/NDI SDK").exists()) {
        ndiSdk = file("C:/Program Files/NDI SDK").toPath()
    }
    if (ndiSdk == null && OperatingSystem.current().isMacOsX && file("../NDI SDK for Apple").exists()) {
        ndiSdk = file("../NDI SDK for Apple").toPath()
    }

    // Check the working directory (repo root)
    if (ndiSdk == null && file("../NDI SDK for Windows").exists()) {
        ndiSdk = file("../NDI SDK for Windows").toPath()
    }
    if (ndiSdk == null && file("../NDI SDK for Linux").exists()) {
        ndiSdk = file("../NDI SDK for Linux").toPath()
    }
    if (ndiSdk == null && file("../NDI SDK for Apple").exists()) {
        ndiSdk = file("../NDI SDK for Apple").toPath()
    }

    if (ndiSdk == null) {
        return null
    }

    return when {
        Files.exists(ndiSdk.resolve("include")) -> ndiSdk.resolve("include")
        Files.exists(ndiSdk.resolve("Include")) -> ndiSdk.resolve("Include")
        else -> throw IllegalStateException("NDI SDK at $ndiSdk is invalid: Has no 'include' or 'Include' subdirectory.")
    }
}

fun locateNdiSdkRoot(): Path? = locateNdiSdk()?.let { it.parent }

val currentOs = when {
    OperatingSystem.current().isMacOsX -> "macos"
    OperatingSystem.current().isLinux -> "linux"
    OperatingSystem.current().isWindows -> "windows"
    else -> throw IllegalStateException("Unsupported host OS for natives build")
}

// Release variant. "allinone" (default) bundles the NDI runtime (libndi) into the jar.
// "system-runtime" ships only this library's own GPL-3 code and JNI bindings, so the jar
// stays free of proprietary NDI binaries and relies on a runtime installed on the user's
// system. The NDI SDK is still required at build time for its headers.
val variant = (findProperty("variant") ?: "allinone").toString()
val bundleNdiRuntime = variant != "system-runtime"

val nativeOutDir = layout.buildDirectory.dir("nativeArtifacts")

data class NativeTarget(val archDir: String, val compilerArgs: List<String>)

val hostTargets: List<NativeTarget> = when (currentOs) {
    // On Apple Silicon, produce both arm64 and x86-64 slices with the same SDK clang
    "macos" -> listOf(
        NativeTarget("arm64-v8a", listOf("-target", "arm64-apple-macos10.15")),
        NativeTarget("x86-64", listOf("-target", "x86_64-apple-macos10.15")),
    )
    else -> listOf(NativeTarget("x86-64", listOf()))
}

val cppSources = fileTree("src/main/cpp") { include("*.cpp") }

val compileNatives by tasks.registering {
    dependsOn(headerOnly, downloadFilesystemHeader)
    inputs.files(cppSources, headerOnly)
    locateNdiSdk()?.let { inputs.dir(it.parent) }
    outputs.dir(nativeOutDir)

    doLast {
        val jdkInclude = jniCompiler.get().executablePath.asFile.parentFile.parentFile.resolve("include")
        val jniHeaders = headerOnly.singleFile
        val ndiIncludes = (locateNdiSdk()?.toFile()
                ?: throw IllegalStateException("No NDI SDK found. Please set the NDI_SDK_DIR variable to the install location, run gradle with -DndiSdk=<Install Path>, or place the SDK in the repository root as \"NDI SDK for Apple\"/\"NDI SDK for Linux\".")).absolutePath
        val fsIncludes = filesystemHeaderDir.get().asFile
        val outRoot = nativeOutDir.get().asFile

        val platformInclude = when (currentOs) {
            "macos" -> "darwin"
            "windows" -> "win32"
            else -> "linux"
        }
        val includeFlags = listOf(
            jniHeaders.absolutePath,
            jdkInclude.absolutePath,
            File(jdkInclude, platformInclude).absolutePath,
            ndiIncludes,
            fsIncludes.absolutePath
        )

        val outName = System.mapLibraryName("ndistream-natives")

        for (target in hostTargets) {
            val outDir = File(outRoot, "natives/$currentOs/${target.archDir}")
            outDir.mkdirs()
            val outFile = File(outDir, outName)

            val command: List<String> = if (currentOs == "windows") {
                buildList {
                    add("cl")
                    add("/std:c++17"); add("/O2"); add("/LD"); add("/EHsc")
                    includeFlags.forEach { add("/I$it") }
                    cppSources.files.forEach { add(it.absolutePath) }
                    add("/link")
                    add("/OUT:${outFile.absolutePath}")
                }
            } else {
                buildList {
                    add(if (currentOs == "macos") "clang++" else "g++")
                    add("-std=c++11"); add("-O3")
                   addAll(target.compilerArgs)
                    add("-fPIC"); add("-shared")
                    includeFlags.forEach { add("-I"); add(it) }
                    cppSources.files.forEach { add(it.absolutePath) }
                    if (currentOs == "linux") {
                        add("-lstdc++"); add("-static-libgcc"); add("-static-libstdc++"); add("-ldl")
                    }
                    add("-o"); add(outFile.absolutePath)
                }
            }

            logger.lifecycle("Compiling ndistream-natives for $currentOs/${target.archDir}")
            project.exec { commandLine(command) }
        }
    }
}

fun ndiRuntimeFiles(archDir: String): List<Pair<File, String>> {
    val sdkRoot = locateNdiSdkRoot() ?: return emptyList()

    return when (currentOs) {
        "macos" -> {
            val lib = sdkRoot.resolve("lib/macOS/libndi.dylib").toFile()
            val license = sdkRoot.resolve("lib/macOS/libndi_licenses.txt").toFile()
            listOfNotNull(
                if (lib.exists()) Pair(lib, "libndi.dylib") else null,
                if (license.exists()) Pair(license, license.name) else null
            )
        }
        "linux" -> {
            val libParent = when (archDir) {
                "x86-64" -> sdkRoot.resolve("lib/x86_64-linux-gnu").toFile()
                else -> sdkRoot.resolve("lib/i686-linux-gnu").toFile()
            }
            val lib = if (libParent.exists()) libParent.listFiles()!!.filter { it.isFile && it.length() > 10_000 }.firstOrNull() else null
            val license = sdkRoot.resolve("licenses/libndi_licenses.txt").toFile()
            listOfNotNull(
                if (lib != null) Pair(lib, "libndi.so") else null,
                if (license.exists()) Pair(license, license.name) else null
            )
        }
        "windows" -> {
            val libName = if (archDir == "x86-64") "Processing.NDI.Lib.x64.dll" else "Processing.NDI.Lib.x86.dll"
            val libDir = if (archDir == "x86-64") "x64" else "x86"
            val lib = sdkRoot.resolve("Bin/$libDir/$libName").toFile()
            val license = sdkRoot.resolve("Bin/$libDir/Processing.NDI.Lib.Licenses.txt").toFile()
            listOfNotNull(
                if (lib.exists()) Pair(lib, "ndi.dll") else null,
                if (license.exists()) Pair(license, license.name) else null
            )
        }
        else -> emptyList()
    }
}

val assembleNativeArtifacts by tasks.registering(Jar::class) {
    dependsOn(compileNatives)
    archiveBaseName.set("ndistream-native-artifacts")
    destinationDirectory.set(layout.buildDirectory.dir("libs"))

    inputs.property("bundleNdiRuntime", bundleNdiRuntime)

    from(nativeOutDir) {
        if (!bundleNdiRuntime) {
            // system-runtime: strip any NDI runtime files (also stale copies in the output dir)
            exclude("**/libndi.*", "**/libndi_licenses.txt", "**/ndi.dll", "**/Processing.NDI.Lib.Licenses.txt")
        }
    }

    doFirst {
        if (!bundleNdiRuntime) {
            logger.lifecycle("Variant 'system-runtime': bundling no NDI runtime; " +
                    "the library will require the system NDI runtime at run-time.")
            return@doFirst
        }
        // Bundle the NDI runtime (the "allinone" build) so the Processing library
        // is self-contained and does not require a separate NDI runtime install.
        for (target in hostTargets) {
            val runtimeFiles = ndiRuntimeFiles(target.archDir)
            if (runtimeFiles.isEmpty()) {
                logger.lifecycle("No bundled NDI runtime found for $currentOs/${target.archDir}; " +
                        "the library will require the system NDI runtime at run-time.")
            }
            for ((file, name) in runtimeFiles) {
                copy {
                    from(file)
                    rename { name }
                    into(File(nativeOutDir.get().asFile, "natives/$currentOs/${target.archDir}"))
                }
            }
        }
    }
}

val nativeArtifacts: Configuration by configurations.creating {
    isCanBeConsumed = true
    isCanBeResolved = false
}

artifacts {
    add("nativeArtifacts", assembleNativeArtifacts)
}
