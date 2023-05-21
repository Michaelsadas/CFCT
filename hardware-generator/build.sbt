
organization := "Fudan University"

version := "1.0.0"

name := "hardware-generator"

scalaVersion := "2.12.10" // "2.13.8"

scalacOptions ++= Seq(
  "-language:reflectiveCalls",
  "-deprecation",
  "-feature",
  "-Xcheckinit",
  "-P:chiselplugin:genBundleElements"
)

addCompilerPlugin("edu.berkeley.cs" % "chisel3-plugin" % "3.5.4" cross CrossVersion.full)

libraryDependencies ++= Seq(
  "edu.berkeley.cs" %% "chisel3" % "3.5.4",
  "edu.berkeley.cs" %% "rocketchip" % "1.2.+",
  "edu.berkeley.cs" %% "chisel-iotesters" % "2.5.4" % "test",
  //  "edu.berkeley.cs" %% "chiseltest" % "0.5.0" % "test"
)

//libraryDependencies += "edu.berkeley.cs" %% "testchipip" % "1.0-020719-SNAPSHOT"

libraryDependencies ++= Seq(
  "com.fasterxml.jackson.core" % "jackson-core" % "2.12.5",
  "com.fasterxml.jackson.core" % "jackson-annotations" % "2.12.5",
  "com.fasterxml.jackson.core" % "jackson-databind" % "2.12.5",
  "com.fasterxml.jackson.module" %% "jackson-module-scala" % "2.12.5"
)

//libraryDependencies ++= Seq(
//  "com.fasterxml.jackson.core" % "jackson-core" % "2.14.0",
//  "com.fasterxml.jackson.core" % "jackson-annotations" % "2.14.0",
//  "com.fasterxml.jackson.core" % "jackson-databind" % "2.14.0",
//  "com.fasterxml.jackson.module" %% "jackson-module-scala" % "2.14.0"
//)

