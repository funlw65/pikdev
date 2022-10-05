/* Informations about external tools used by pikdev
   (this is a singleton class) 
   @author Alain Gibaud (free.fr) <alain.gibaud@free.fr> */

#ifndef TOOLCOLLECTION_H
#define TOOLCOLLECTION_H

#include <QString>
#include <QMap>
#include <QString>
#include <QStringList>


struct ExternalTool // data internally used by Toolcollection class
{
    QString tool_name;
    QString tool_path;
    QString tool_URL;
    QString tool_informations;
    bool tool_mandatory;
};

class ToolCollection
{
    QMap<QString, ExternalTool> collection;	
    static ToolCollection myself;

public:
    static ToolCollection *toolCollection ();
    ToolCollection ();
    ~ToolCollection ();
    void addTool (const QString& name, const QString& url, const QString& info,
		  bool mand = false);
    QString path (const QString& name);
    QString URL (const QString& name);
    QString info (const QString& name);
    bool isMandatory (const QString& name);
    bool exist (const QString& name);
    QStringList toolNames ();
    void setPath (const QString& toolname, const QString& toolpath);
};

#endif
