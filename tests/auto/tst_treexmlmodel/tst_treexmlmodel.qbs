import "../main.qbs" as TreeXmlModelProduct

TreeXmlModelProduct {
    name: "tst_treexmlmodel"

    productname: ""
    version:     "1.0.0"
    description: ""
    company:     "RTPTechGroup"
    copyright:   "Copyright (C) RTPTechGroup"

    Depends { name: "Qt"; submodules: ["widgets", "xml", "testlib"] }
    Depends { name: "treexmlmodel" }
    Depends { name: "modeltest" }

    files: [
        "*.h",
        "*.cpp",
        "*.ui",
        "*.qrc"
    ]
}
