#include <QtCore/QtGlobal>

#if defined(XMLMODEL_LIBRARY)
#  define XMLMODELLIB Q_DECL_EXPORT
#else
#  define XMLMODELLIB Q_DECL_IMPORT
#endif
