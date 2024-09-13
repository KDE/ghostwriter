#ifndef OPEN_ACTION_H
#define OPEN_ACTION_H

#include "../markdowndocument.h"

namespace ghostwriter
{

class OpenAction
{
public:
    OpenAction(bool saveBackup);
    ~OpenAction();

    MarkdownDocument *openFile();
    MarkdownDocument *openDraft();
};

} // namespace ghostwriter

#endif // OPEN_ACTION_H
