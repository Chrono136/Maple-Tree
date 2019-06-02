#include "ticket.h"

Ticket::Ticket(QObject* parent) : QObject(parent) {
  len = 0;
}

void Ticket::Create(const QString id, const QString key, const QString ver, const QString directory) {
  if (!id.isEmpty() && !key.isEmpty()) {
    auto ticket = new Ticket;

    ticket->data.insert(0x1E6, QByteArray::fromHex(ver.toLatin1()));
    ticket->data.insert(0x1BF, QByteArray::fromHex(key.toLatin1()));
    ticket->data.insert(0x2CC, QByteArray::fromHex(id.toLatin1()));
    ticket->len = static_cast<qulonglong>(ticket->data.size());

    QFile file(directory + "/cetk");
    if (!file.open(QIODevice::WriteOnly)) {
      QMessageBox::information(nullptr, "Ticket::Create():", file.errorString());
      return;
    }
    file.write(ticket->data, static_cast<qint64>(ticket->len));
    file.close();

    return;
  }
}
