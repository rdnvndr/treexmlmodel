import "./main.qbs" as LibProduct

LibProduct {
    name: "treexmlmodel"

    productname: "TreeXmlModel Library"
    version:     "1.0.0"
    description: "TreeXmlModel Library"
    company:     "RTPTechGroup"
    copyright:   "Copyright (C) RTPTechGroup"

    cpp.defines: base.concat(["XMLMODEL_LIBRARY"])
    Depends { name: "Qt"; submodules: ["widgets", "xml"] }

    files: [
        "*.h",
        "*.cpp",
        "*.ui",
        "*.qrc"
    ]
}

