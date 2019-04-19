#include "ticket.h"

Ticket::Ticket(QObject* parent) : QObject(parent) {
  len = 0;
}

void Ticket::Create(TitleInfo* ti) {
  if (ti && !ti->getID().isEmpty() && !ti->getKey().isEmpty()) {
    auto ticket = new Ticket;

    // ticket->data.insert(0x1E6,
    // QByteArray::fromHex(ti->getVersions().toLatin1()));
    ticket->data.insert(0x1BF, QByteArray::fromHex(ti->getKey().toLatin1()));
    ticket->data.insert(0x2CC, QByteArray::fromHex(ti->getID().toLatin1()));
    ticket->len = static_cast<qulonglong>(ticket->data.size());

    QString cetkpath(ti->getDirectory() + "/cetk");
    QFile file(cetkpath);
    if (!file.open(QIODevice::WriteOnly)) {
      QMessageBox::information(nullptr, "Ticket::Save():", file.errorString());
      return;
    }
    file.write(ticket->data, static_cast<qint64>(ticket->len));
    file.close();

    return;
  }
}
