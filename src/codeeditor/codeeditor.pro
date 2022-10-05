HEADERS = CodeEditor.h \
    CHighlighter.h \
    P18Highlighter.h \
    HighlighterBase.h \
    SHlFactory.h \
    QBEditor.h \
    Appli.h \
    QBEditorSearchDialog.h \
    QBEditorDocument.h \
    Utility.h \
    ItemSelector.h
SOURCES = main.cpp \
    CodeEditor.cpp \
    CHighlighter.cpp \
    P18Highlighter.cpp \
    HighlighterBase.cpp \
    SHlFactory.cpp \
    QBEditor.cpp \
    Appli.cpp \
    QBEditorSearchDialog.cpp \
    QBEditorDocument.cpp \
    Utility.cpp \
    ItemSelector.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/codeeditor
sources.files = $$SOURCES \
    $$HEADERS \
    *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/widgets/codeeditor
INSTALLS += target \
    sources
RESOURCES += QBEditor.qrc
