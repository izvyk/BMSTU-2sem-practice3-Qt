#ifndef BOOK_H
#define BOOK_H

#include <QString>
#include <QDataStream>

struct Book {
private:
    QString _author;
    QString _name;
    QString _publishing;
    unsigned _yearOfPublishing;
    unsigned _quantity;
public:
    Book(QString const &author = "", QString const &name = "",
         QString const &publishing = "",
         unsigned const yearOfPublishing = 0, unsigned const quantity = 0):
        _author(author), _name(name), _publishing(publishing),
        _yearOfPublishing(yearOfPublishing), _quantity(quantity) {}

    QString const& author() const {
        return _author;
    }

    QString const& name() const {
        return _name;
    }

    QString const& publishing() const {
        return _publishing;
    }

    unsigned yearOfPublishing() const {
        return _yearOfPublishing;
    }

    unsigned quantity() const {
        return _quantity;
    }

    void setAuthor(QString const &author) {
        this->_author = author;
    }

    void setName(QString const &name) {
        this->_name = name;
    }

    void setPublishing(QString const &publishing) {
        this->_publishing = publishing;
    }

    void setYearOfPublishing(unsigned const year_of_publishing) {
        this->_yearOfPublishing = year_of_publishing;
    }

    void setQuantity(unsigned const quantity) {
        this->_quantity = quantity;
    }

    friend QDataStream& operator<<(QDataStream &out, Book const& book) {
        out << book._author
            << book._name
            << book._publishing
            << book._yearOfPublishing
            << book._quantity;
        return out;
    }

    friend QDataStream& operator>>(QDataStream &in, Book &book) {
        in >> book._author
           >> book._name
           >> book._publishing
           >> book._yearOfPublishing
           >> book._quantity;
        return in;
    }
};

#endif // BOOK_H
