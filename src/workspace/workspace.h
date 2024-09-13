#ifndef WORKSPACE_H
#define WORKSPACE_H

#include "../document/markdowndocument.h"

namespace ghostwriter
{
class Workspace
{
public:
    Workspace();
    ~Workspace();

    bool setSaveDraftsEnabled();
    bool setBackupsEnabled();

    bool setAutoReloadEnabled();

    void open();
    void saveAll();
};
} // namespace ghostwriter

#endif // WORKSPACE_H