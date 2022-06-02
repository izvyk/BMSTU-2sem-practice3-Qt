#include "BookView.h"

#include <QWidget>
#include <QLabel>
#include <QString>
#include <QIntValidator>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QDate>


QLineEdit* BookView::addLineItem(QBoxLayout* const where, QWidget* const parent, QString const& key, QString const& value) {
    QHBoxLayout* tmpLayout = new QHBoxLayout(parent);
    tmpLayout->setMargin(0);
    QLabel* label = new QLabel(parent);
    label->setText(key);

    QLineEdit* lineEdit = new QLineEdit(parent);
    lineEdit->setText(value);
    lineEdit->setReadOnly(this->_isReadOnly);
    lineEdit->setAlignment(Qt::AlignRight);

    tmpLayout->addWidget(label);
    tmpLayout->addWidget(lineEdit);
    where->addLayout(tmpLayout);
    return lineEdit;
}

void BookView::initLineEdits() {
    _authorEdit = addLineItem(_uiLayout, this, tr("Author"), _book->author());
    _nameEdit = addLineItem(_uiLayout, this, tr("Name"), _book->name());
    auto tmpYear = _book->yearOfPublishing();
    _yearOfPublishingEdit = addLineItem(_uiLayout, this, tr("Year of publishing"), tmpYear ? QString::number(tmpYear) : "");
    _publishingEdit = addLineItem(_uiLayout, this, tr("Publishing"), _book->publishing());
    auto tmpQuantity = _book->quantity();
    _quantityEdit = addLineItem(_uiLayout, this, tr("Quantity"), tmpQuantity ? QString::number(tmpQuantity) : "");

    auto yearOfPublishingValidator = new QIntValidator(_yearOfPublishingEdit);
    _yearOfPublishingEdit->setValidator(yearOfPublishingValidator);
    yearOfPublishingValidator->setTop(QDate::currentDate().year());

    auto quantityValidator = new QIntValidator(_quantityEdit);
    _quantityEdit->setValidator(quantityValidator);
    quantityValidator->setBottom(1);

    _authorEdit->setPlaceholderText(tr("Author`s name (string)"));
    _nameEdit->setPlaceholderText(tr("Book`s name (string)"));
    _yearOfPublishingEdit->setPlaceholderText(tr("Year of publishing (integer)"));
    _publishingEdit->setPlaceholderText(tr("Name of publishing"));
    _quantityEdit->setPlaceholderText(tr("Quantity (positive or zero)"));

    connect(_authorEdit, &QLineEdit::textChanged,
            this, &BookView::syncSubmitButtonClickability);
    connect(_nameEdit, &QLineEdit::textChanged,
            this, &BookView::syncSubmitButtonClickability);
    connect(_yearOfPublishingEdit, &QLineEdit::textChanged,
            this, &BookView::syncSubmitButtonClickability);
    connect(_publishingEdit, &QLineEdit::textChanged,
            this, &BookView::syncSubmitButtonClickability);
    connect(_quantityEdit, &QLineEdit::textChanged,
            this, &BookView::syncSubmitButtonClickability);
}

void BookView::initButtons() {
    _deleteButton = new QPushButton(this);
    _deleteButton->setText(tr("Delete"));
    _editButton = new QPushButton(this);
    _editButton->setText(tr("Edit"));

    connect(_deleteButton, &QPushButton::clicked,
            this, [this]() {
                emit BookView::deleteButtonClicked(this);
            });
    connect(_editButton, &QPushButton::clicked,
            this, [this]() {
                this->setReadOnly(!this->_isReadOnly);
                if (this->_isReadOnly)
                    emit BookView::submited();
    });
}

void BookView::syncSubmitButtonClickability() {
    this->_editButton->setEnabled(this->validate());
}

BookView::BookView(Book * const book, QWidget* parent): QWidget(parent), _book(book) {
    this->_isReadOnly = true;
    this->_uiLayout = new QVBoxLayout(this);

    initLineEdits();
    initButtons();
    syncSubmitButtonClickability();

    QHBoxLayout* buttonsLayout = new QHBoxLayout(this);
    buttonsLayout->addWidget(_deleteButton);
    buttonsLayout->addWidget(_editButton);

    _uiLayout->addLayout(buttonsLayout);

    _uiLayout->setMargin(0);
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    this->setMinimumWidth(200);
}

void BookView::setReadOnly(bool ro) {
    this->_isReadOnly = ro;
    if (this->_isReadOnly) {
        this->_book->setAuthor(_authorEdit->text());
        this->_book->setName(_nameEdit->text());
        this->_book->setPublishing(_publishingEdit->text());
        this->_book->setYearOfPublishing(_yearOfPublishingEdit->text().toUInt());
        this->_book->setQuantity(_quantityEdit->text().toUInt());

        _editButton->setText(tr("Edit"));
    }
    else {
        _editButton->setText(tr("Submit"));
        this->_authorEdit->setFocus();
        this->_authorEdit->selectAll();
    }

    _authorEdit->setReadOnly(this->_isReadOnly);
    _nameEdit->setReadOnly(this->_isReadOnly);
    _publishingEdit->setReadOnly(this->_isReadOnly);
    _yearOfPublishingEdit->setReadOnly(this->_isReadOnly);
    _quantityEdit->setReadOnly(this->_isReadOnly);
}
