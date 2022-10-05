#include "Config.h"
#include "ToolCollection.h"


ToolCollection ToolCollection::myself;

ToolCollection::ToolCollection ()
{
}

ToolCollection::~ToolCollection ()
{
}

ToolCollection *ToolCollection::toolCollection () 
{
    return &myself;
}

void ToolCollection::addTool (const QString& name, const QString& url,
			      const QString& info, bool mand)
{
    ExternalTool tool;
    tool.tool_name = name;
    tool.tool_URL = url;
    tool.tool_informations = info;
    tool.tool_mandatory = mand;
    Config::getToolPath (name, tool.tool_path);

    collection[name] = tool;
}

QString ToolCollection::path (const QString& name)
{
    return collection[name].tool_path;
}

QString ToolCollection::URL (const QString& name)
{
    return collection[name].tool_URL;
}

QString ToolCollection::info (const QString& name)
{
    return collection[name].tool_informations;
}

bool ToolCollection::isMandatory (const QString& name)
{
    return collection[name].tool_mandatory;
}

bool ToolCollection::exist (const QString& name)
{
    return collection.find (name) != collection.end ();
}

QStringList ToolCollection::toolNames ()
{
    QStringList names;
    QMap<QString, ExternalTool>::iterator i;
    for (i = collection.begin (); i != collection.end (); ++i)
      names += (*i).tool_name;

    return names;
}

void ToolCollection::setPath (const QString& toolname, const QString& toolpath)
{
    if (exist (toolname))
      collection[toolname].tool_URL = toolpath;
}
