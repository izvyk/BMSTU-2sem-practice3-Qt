#ifndef BOOKVIEW_H
#define BOOKVIEW_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>

#include "book.h"


class BookView: public QWidget {
    Q_OBJECT
public:
    BookView(Book* const book, QWidget* parent = nullptr);

    Book const * book() const {
        return _book;
    }

    bool validate() const {
        return
            _authorEdit->hasAcceptableInput() &&
            _nameEdit->hasAcceptableInput() &&
            _publishingEdit->hasAcceptableInput() &&
            _yearOfPublishingEdit->hasAcceptableInput() &&
            _quantityEdit->hasAcceptableInput();
    }

private:
    Book * const _book;

    QVBoxLayout *_uiLayout;
    QPushButton *_editButton;
    QPushButton *_deleteButton;

    QLineEdit
        *_authorEdit,
        *_nameEdit,
        *_publishingEdit,
        *_yearOfPublishingEdit,
        *_quantityEdit;

    bool _isReadOnly;

    QLineEdit* addLineItem(QBoxLayout* const where, QWidget* const parent, QString const& key, QString const& value);
    void initLineEdits();
    void initButtons();

signals:
    void deleteButtonClicked(BookView *);
    void submited();

private slots:
    void syncSubmitButtonClickability();

public slots:
    void setReadOnly(bool ro);

    void authorEditSetFocus() {
        this->_authorEdit->setFocus();
    }
};

#endif // BOOKVIEW_H
