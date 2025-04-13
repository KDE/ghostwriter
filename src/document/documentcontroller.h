#include <QObject>

class DocumentController : public QObject
{
public:
    static DocumentController *openDocument(const QWidget *parent = nullptr);

    ~DocumentController();

private:
    DocumentController(const QWidget *parent = nullptr);
};