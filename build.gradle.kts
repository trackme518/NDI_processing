// This Gradle script builds NDI_p5 as a Processing 4 library,
// following the processing-library-template conventions:
// https://processing.github.io/processing-library-template/getting-started.html
//
// In addition to the template tasks (buildReleaseArtifacts, deployToProcessingSketchbook),
// this build compiles the JNI natives in the "ndip5-natives" subproject and bundles them
// (together with the NDI runtime) inside the library jar.

import java.util.Properties
import org.gradle.internal.os.OperatingSystem

plugins {
    id("java")
}

// Sets the Java version to use for compiling the library.
// Processing 4 was compiled with Java 17, so the library is compiled with 17 as well.
java {
    toolchain {
        languageVersion = JavaLanguageVersion.of(17)
    }
}

// read in user-defined properties in release.properties file
// most of these properties will be saved to the library.properties file, a required file in the release
// using task writeLibraryProperties
val libraryProperties = Properties().apply {
    load(rootProject.file("release.properties").inputStream())
}

// the following conditional allows for the version to be overwritten by a Github release
// via the release workflow, which defines a property named "githubReleaseTag"
version = if (project.hasProperty("githubReleaseTag")) {
    // remove leading "v" from tag (the leading "v" is required for the release workflow to trigger)
    project.property("githubReleaseTag").toString().drop(1)
} else {
    libraryProperties.getProperty("prettyVersion")
}

//==========================
// USER BUILD CONFIGURATIONS
//==========================

// the short name of the library. This string will name relevant files and folders.
// Such as:
// <libName>.jar will be the name of your build jar
// <libName>.zip will be the name of your release file
val libName = "NDI_p5"

// The release variant:
//   allinone       - the jar bundles the NDI runtime (libndi) - works out of the box,
//                    but the shipped binaries mix GPL-3 code with proprietary NDI binaries.
//   system-runtime - fully GPL-3: the jar ships only this library's own code and JNI
//                    bindings; the NDI runtime must be installed on the user's system.
// Select with -Pvariant=system-runtime (default is allinone).
val variant = (project.findProperty("variant") ?: "allinone").toString()
require(variant == "allinone" || variant == "system-runtime") {
    "Unknown variant '$variant' - must be 'allinone' or 'system-runtime'"
}

// The group ID of the library, which uniquely identifies the project.
group = "p5"

// The location of your sketchbook folder. The sketchbook folder holds your installed
// libraries, tools, and modes.
var sketchbookLocation = ""
val userHome = System.getProperty("user.home")
val currentOS = OperatingSystem.current()
if (currentOS.isMacOsX) {
    sketchbookLocation = if (File("$userHome/Documents/Processing/sketchbook").isDirectory) {
        "$userHome/Documents/Processing/sketchbook"
    } else {
        "$userHome/Documents/Processing"
    }
} else if (currentOS.isWindows) {
    val docsFolder = if (File("$userHome/My Documents").isDirectory) {
        "$userHome/My Documents"
    } else {
        "$userHome/Documents"
    }
    sketchbookLocation = if (File(docsFolder, "Processing/sketchbook").isDirectory) {
        "$docsFolder/Processing/sketchbook"
    } else {
        "$docsFolder/Processing"
    }
} else {
    sketchbookLocation = "$userHome/sketchbook"
}

// Repositories where dependencies will be fetched from.
repositories {
    mavenCentral()
}

dependencies {
    // NDI_p5 itself has no runtime dependencies - the NDI runtime and JNI bindings
    // are bundled inside the library jar by the "ndip5-natives" subproject.

    testImplementation(platform("org.junit:junit-bom:5.10.0"))
    testImplementation("org.junit.jupiter:junit-jupiter")
}

//==============================
// END USER BUILD CONFIGURATIONS
//==============================


// =============================
// INTERNAL BUILD CONFIGURATIONS
// Do not edit the following sections unless you know what you're doing.
// =============================

// Settings for how the JAR file (your library) will be built.
tasks.jar {
    archiveBaseName.set(libName)
    archiveClassifier.set("")
    archiveVersion.set("")
}

tasks.test {
    useJUnitPlatform()
}

// =============================
// JNI header generation + native library bundling
// =============================

val javaToolchains = project.extensions.getByType(JavaToolchainService::class)
val jniCompiler = javaToolchains.compilerFor {
    languageVersion.set(JavaLanguageVersion.of(17))
}

// Generate the C headers for the JNI bindings (consumed by the ndip5-natives subproject)
val generateJniHeaders by tasks.registering {
    description = "Generates C headers for the NDI_p5 JNI bindings with javac -h"
    group = "build"

    val headersDir = layout.buildDirectory.dir("generated/jniHeaders")
    val classesDir = layout.buildDirectory.dir("generated/jniHeaderClasses")

    inputs.files(sourceSets.main.get().allJava)
    outputs.dir(headersDir)

    doLast {
        val headers = headersDir.get().asFile
        val classes = classesDir.get().asFile
        headers.mkdirs()
        classes.mkdirs()

        val javac = File(jniCompiler.get().executablePath.asFile.parentFile, "javac")
        val sources = inputs.files.files.map { it.absolutePath }

        // Run javac -h purely for header generation; compilation errors are not expected
        project.exec {
            commandLine(listOf(javac.absolutePath, "-h", headers.absolutePath, "-d", classes.absolutePath) + sources)
        }
    }
}

val jniIncludes: Configuration by configurations.creating {
    isCanBeConsumed = true
    isCanBeResolved = false
}

artifacts {
    add("jniIncludes", generateJniHeaders)
}

// Pull the compiled native libraries (and the NDI runtime) from the ndip5-natives subproject
val nativeArtifacts: Configuration by configurations.creating

dependencies {
    nativeArtifacts(project(":ndip5-natives", "nativeArtifacts"))
}

tasks.jar {
    dependsOn(nativeArtifacts)
    from(nativeArtifacts.map { zipTree(it) })
}

// ===========================
// Tasks for releasing library
// ===========================

val releaseRoot = "$rootDir/release"
val releaseName = libName
val releaseDirectory = "$releaseRoot/$releaseName"

tasks.register<WriteProperties>("writeLibraryProperties") {
    group = "processing"
    destinationFile = project.file("library.properties")

    property("name", libraryProperties.getProperty("name"))
    property("version", libraryProperties.getProperty("version"))
    property("prettyVersion", project.version)
    property("authors", libraryProperties.getProperty("authors"))
    property("url", libraryProperties.getProperty("url"))
    property("categories", libraryProperties.getProperty("categories"))
    property("sentence", libraryProperties.getProperty("sentence"))
    val variantNote = if (variant == "system-runtime")
        " This is the 'system-runtime' variant: it contains only GPL-3 licensed code and requires the free NDI Runtime to be installed on your system."
    else
        " This is the default 'allinone' variant: the NDI runtime is bundled for out-of-the-box use."
    property("paragraph", libraryProperties.getProperty("paragraph") + variantNote)
    property("minRevision", libraryProperties.getProperty("minRevision"))
    property("maxRevision", libraryProperties.getProperty("maxRevision"))
}

// define the order of running, to ensure clean is run first
tasks.build.get().mustRunAfter("clean")
tasks.javadoc.get().mustRunAfter("build")

tasks.register("buildReleaseArtifacts") {
    group = "processing"
    dependsOn("clean", "build", "javadoc", "writeLibraryProperties")
    finalizedBy("packageRelease", "duplicateZipToPdex")

    doFirst {
        println("Releasing library $libName")
        println(org.gradle.internal.jvm.Jvm.current())

        println("Cleaning release...")
        project.delete(files(releaseRoot))
    }

    doLast {
        println("Creating package...")

        println("Copy library...")
        copy {
            from(layout.buildDirectory.file("libs/${libName}.jar"))
            into("$releaseDirectory/library")
        }

        println("Copy dependencies...")
        copy {
            from(configurations.runtimeClasspath)
            into("$releaseDirectory/library")
        }

        println("Copy assets...")
        copy {
            from("$rootDir")
            include("shaders/**", "native/**")

            into("$releaseDirectory/library")
            exclude("*.DS_Store")
        }

        println("Copy javadoc...")
        copy {
            from(layout.buildDirectory.dir("docs/javadoc"))
            into("$releaseDirectory/reference")
        }

        println("Copy additional artifacts...")
        copy {
            from(rootDir)
            include("README.md", "readme/**", "library.properties", "LICENSE", "examples/**", "src/**")

            into(releaseDirectory)
            exclude("*.DS_Store", "**/networks/**")
        }

        println("Copy repository library.txt...")
        copy {
            from(rootDir)
            include("library.properties")
            into(releaseRoot)
            rename("library.properties", "$libName.txt")
        }
    }
}

tasks.register<Zip>("packageRelease") {
    dependsOn("buildReleaseArtifacts")
    doFirst {
        println("Create zip file...")
    }
    archiveFileName.set("${libName}-${variant}.zip")
    from(releaseDirectory)
    into(releaseName)
    destinationDirectory.set(file(releaseRoot))
    exclude("**/*.DS_Store")
}

tasks.register<Copy>("duplicateZipToPdex") {
    doFirst {
        println("Duplicate zip file to pdex extension...")
    }
    from(releaseRoot) {
        include("$libName-${variant}.zip")
        rename("$libName-${variant}.zip", "$libName-${variant}.pdex")
    }
    into(releaseRoot)
}
tasks["duplicateZipToPdex"].mustRunAfter("packageRelease")

tasks.register("deployToProcessingSketchbook") {
    group = "processing"
    dependsOn("buildReleaseArtifacts")

    doFirst {
        println("Copy to sketchbook  $sketchbookLocation ...")
    }

    doLast {
        val installDirectory = file("$sketchbookLocation/libraries/$libName")

        println("Removing old install from: $installDirectory")
        delete(installDirectory)

        println("Copying fresh build to sketchbook $sketchbookLocation ...")
        copy {
            from(releaseDirectory)
            include(
                "library.properties",
                "examples/**",
                "library/**",
                "reference/**",
                "src/**"
            )
            into(installDirectory)
        }
    }
}
