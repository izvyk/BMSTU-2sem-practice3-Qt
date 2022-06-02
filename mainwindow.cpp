#include "mainwindow.h"

#include <QMainWindow>
#include <QScrollArea>
#include <QChartView>
#include <QBarSeries>
#include <QBarSet>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include <QScreen>
#include <QGuiApplication>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QListWidget>
#include <QSpinBox>
#include <QMap>
#include <QMultiMap>
#include <QSet>

#include "BookView.h"

#define SERIALIZE_VERSION QDataStream::Qt_5_9
#define CHART_AXIS_MAX_TICKS 8


MainWindow::MainWindow(QWidget *parent): QMainWindow(parent) {
    _addingBook = false;
    _booksLayout = new QVBoxLayout(this);
    _booksLayout->setAlignment(Qt::AlignTop);
    _tabsWidget = new QTabWidget(this);


    this->initBookListTab();
    this->initChartViewTab();
    this->initUniqueBooksTab();
    this->initPeriodTab();
    this->initChronoTab();


    this->setCentralWidget(_tabsWidget);
    this->setWindowTitle(tr("Practice #3"));
    this->resize(QGuiApplication::primaryScreen()->availableGeometry().size() * 0.65);
    this->syncAllControlsClickability();


    connect(this, &MainWindow::addBookStarted,
            this, [this] { this->_addingBook = true; });
    connect(this, &MainWindow::addBookFinished,
            this, [this] { this->_addingBook = false; });

    connect(this, &MainWindow::loadedFromFile,
            this, &MainWindow::syncAllControlsClickability);
    connect(this, &MainWindow::loadedWithoutErrors,
            this, [this] {
                QMessageBox::information(this, tr("Info"), tr("Successfully loaded!"));
    });
    connect(this, &MainWindow::loadedWithErrors,
            this, [this] {
                QMessageBox::warning(this,
                    tr("Damaged file"), tr("File contains invalid books.\nThey were ignored"));
    });

    connect(this, &MainWindow::addBookStarted,
            this, &MainWindow::syncAllControlsClickability);
    connect(this, &MainWindow::addBookFinished,
            this, &MainWindow::syncAllControlsClickability);
    connect(this, &MainWindow::bookDeleted,
            this, &MainWindow::syncAllControlsClickability);
    connect(this, &MainWindow::bookAdded,
            this, &MainWindow::syncSaveButtonClickability);

    connect(_tabsWidget, &QTabWidget::currentChanged,
            this, &MainWindow::syncAllTabs);
}

void MainWindow::initBookListTab() {
    auto bookListWidget = new QWidget(_tabsWidget);
    QScrollArea *area = new QScrollArea(bookListWidget);
    auto areaWidget = new QWidget(area);
    areaWidget->setLayout(_booksLayout);
    area->setWidget(areaWidget);
    area->setWidgetResizable(true);

    auto bookListLayout = new QVBoxLayout(bookListWidget);
    bookListWidget->setLayout(bookListLayout);

    auto bookListButtonsLayout = new QHBoxLayout(bookListWidget);
    this->initBookListButtons();

    bookListButtonsLayout->addWidget(_addBookButton);
    bookListButtonsLayout->addWidget(_saveButton);
    bookListButtonsLayout->addWidget(_loadButton);


    bookListLayout->addLayout(bookListButtonsLayout);
    bookListLayout->addWidget(area);
    _tabsWidget->addTab(bookListWidget, tr("All books"));
}

void MainWindow::initUniqueBooksTab() {
    _uniqueBooksListWidget = new QListWidget(this);
    _tabsWidget->addTab(_uniqueBooksListWidget, tr("Unique books"));
}

void MainWindow::initPeriodTab() {
    _periodWidget = new QWidget(_tabsWidget);

    auto verticalLayout = new QVBoxLayout(_periodWidget);
    auto controlsLayout = new QHBoxLayout(_periodWidget);

    _periodLowerSpinBox = new QSpinBox(_periodWidget);
    _periodHigherSpinBox = new QSpinBox(_periodWidget);
    controlsLayout->addWidget(_periodLowerSpinBox);
    controlsLayout->addWidget(_periodHigherSpinBox);

    _periodListWidget = new QListWidget(_periodWidget);

    verticalLayout->addLayout(controlsLayout);
    verticalLayout->addWidget(_periodListWidget);

    _periodWidget->setLayout(verticalLayout);
    _tabsWidget->addTab(_periodWidget, "Authors by period");


    connect(_periodLowerSpinBox, qOverload<int>(&QSpinBox::valueChanged),
            this, [this] {
                if (_periodHigherSpinBox->value() < _periodLowerSpinBox->value())
                    _periodHigherSpinBox->setValue(_periodLowerSpinBox->value());
    });
    connect(_periodHigherSpinBox, qOverload<int>(&QSpinBox::valueChanged),
            this, [this] {
                if (_periodLowerSpinBox->value() > _periodHigherSpinBox->value())
                    _periodLowerSpinBox->setValue(_periodHigherSpinBox->value());
    });
    connect(_periodLowerSpinBox, qOverload<int>(&QSpinBox::valueChanged),
            this, &MainWindow::syncPeriodList);
    connect(_periodHigherSpinBox, qOverload<int>(&QSpinBox::valueChanged),
            this, &MainWindow::syncPeriodList);
}

void MainWindow::initChronoTab() {
    _chronoWidget = new QWidget(this);
    auto chronoLayout = new QVBoxLayout(_chronoWidget);
    _chronoWidget->setLayout(chronoLayout);
    _publishingComboBox = new QComboBox(_chronoWidget);
    _chronoList = new QListWidget(_chronoWidget);
    chronoLayout->addWidget(_publishingComboBox);
    chronoLayout->addWidget(_chronoList);

    _tabsWidget->addTab(_chronoWidget, "Chono");

    connect(_publishingComboBox, &QComboBox::currentTextChanged,
            this, &MainWindow::syncChronoList);
}

void MainWindow::initChartViewTab() {

    auto *chart = new QtCharts::QChart();
    _axisX = new QtCharts::QBarCategoryAxis(chart);
    _axisY = new QtCharts::QValueAxis(chart);
    _series = new QtCharts::QBarSeries(chart);


    chart->addSeries(_series);
    chart->addAxis(_axisY, Qt::AlignLeft);
    chart->addAxis(_axisX, Qt::AlignBottom);

    chart->legend()->hide();
    chart->setTitle(tr("Published books"));
    chart->setAnimationOptions(QtCharts::QChart::SeriesAnimations);
    _axisX->setTitleText("Year");
    _axisY->setTitleText("Quantity");
    _axisY->setLabelFormat("%d");


    _chartView = new QtCharts::QChartView(chart);
    _chartView->setRenderHint(QPainter::Antialiasing);


    _tabsWidget->addTab(_chartView, tr("Chart"));
}

void MainWindow::initBookListButtons() {
    _saveButton = new QPushButton(tr("Save"), this);
    _loadButton = new QPushButton(tr("Load"), this);
    _addBookButton = new QPushButton(tr("Add a book"), this);

    connect(_saveButton, &QPushButton::clicked,
            this, [this] {
                static QString lastSaveFileName = "books.pr3";
                QString saveFileName = QFileDialog::getSaveFileName(this,
                    tr("Save books"), lastSaveFileName,
                    tr("Practice #3 (*.pr3);;All Files (*)"));
                if (saveFileName.isEmpty()) {
                    QMessageBox::information(this, tr("Info"), tr("No file is selected!"));
                }
                else {
                    saveToFile(saveFileName);
                    lastSaveFileName = saveFileName;
                    QMessageBox::information(this, tr("Info"), tr("Successfully saved!"));
                }
    });
    connect(_loadButton, &QPushButton::clicked,
            this, [this] {
                QString loadFileName = QFileDialog::getOpenFileName(this,
                    tr("Load books"), "books.pr3",
                    tr("Practice #3 (*.pr3);;All Files (*)"));
                if (loadFileName.isEmpty()) {
                    QMessageBox::information(this, tr("Info"), tr("No file is selected!"));
                }
                else {
                    loadFromFile(loadFileName);
                }
    });
    connect(_addBookButton, &QPushButton::clicked,
            this, [this] {
                emit this->addBookStarted();
                auto newBookView = new BookView(new Book(), this);

                newBookView->setReadOnly(false);
                newBookView->setVisible(false);
                this->_booksLayout->insertWidget(0, newBookView);
                newBookView->setVisible(true);
                newBookView->authorEditSetFocus();

                auto const submitButtonConnection = new QMetaObject::Connection;
                auto const deleteButtonConnection = new QMetaObject::Connection;

                auto const removeConnections = [submitButtonConnection, deleteButtonConnection] {
                    disconnect(*submitButtonConnection);
                    disconnect(*deleteButtonConnection);
                    delete submitButtonConnection;
                    delete deleteButtonConnection;
                };
                *submitButtonConnection = connect(newBookView, &BookView::submited,
                                                  _addBookButton, [removeConnections, this] {
                    emit this->addBookFinished();
                    emit this->bookAdded();
                    removeConnections();
                });
                *deleteButtonConnection = connect(newBookView, &BookView::deleteButtonClicked,
                                                  _addBookButton, [removeConnections, this] {
                    emit this->addBookFinished();
                    removeConnections();
                });
                connect(newBookView, &BookView::deleteButtonClicked,
                        this, &MainWindow::removeBook);
    });
}

void MainWindow::syncChart() {
    QMap<unsigned, unsigned> quantityByYears;
    Book const * book;
    for (int i = 0; i < this->_booksLayout->count(); ++i) {
        book = static_cast<BookView *>(_booksLayout->itemAt(i)->widget())->book();
        if (quantityByYears.contains(book->yearOfPublishing()))
            quantityByYears[book->yearOfPublishing()] += book->quantity();
        else
            quantityByYears[book->yearOfPublishing()] = book->quantity();
    }

    _series->clear();
    _axisX->clear();

    auto newset = new QtCharts::QBarSet("", this);

    unsigned maxQuantity = 0;
    unsigned quantity;
    foreach (unsigned const& year, quantityByYears.keys()) {
        quantity = quantityByYears.value(year);
        *newset << quantity;
        _axisX->append(QString::number(year));
        maxQuantity = maxQuantity > quantity ? maxQuantity : quantity;
    }

    _series->append(newset);
    _series->attachAxis(_axisX);
    _series->attachAxis(_axisY);

    _axisY->setMax(maxQuantity);
    _axisY->setTickCount(maxQuantity > CHART_AXIS_MAX_TICKS - 1 ?
        CHART_AXIS_MAX_TICKS : (maxQuantity % CHART_AXIS_MAX_TICKS) + 1);
    _axisY->setMinorTickCount(1);
    _axisX->setRange(QString::number(quantityByYears.firstKey()),
                     QString::number(quantityByYears.lastKey()));
}

void MainWindow::syncUniqueBooksList() {
    _uniqueBooksListWidget->clear();

    if (_booksLayout->count() == 0)
        return;

    Book const *book;
    for (int i = 0; i < _booksLayout->count(); ++i) {
        book = static_cast<BookView *>(this->_booksLayout->itemAt(i)->widget())->book();
        if (book->quantity() == 1) {
            _uniqueBooksListWidget->addItem(book->name());
        }
    }
}

void MainWindow::syncPublishingComboBox() {
    _publishingComboBox->clear();
    QSet<QString const> publishingList;

    Book const *book;
    for (int i = 0; i < this->_booksLayout->count(); ++i) {
        book = static_cast<BookView *>(_booksLayout->itemAt(i)->widget())->book();
        if (!publishingList.contains(book->publishing())) {
            publishingList << book->publishing();
            _publishingComboBox->addItem(book->publishing());
        }
    }
}

void MainWindow::syncPeriodSpinBoxes() {
    if (_booksLayout->count() == 0)
        return;

    unsigned year = static_cast<BookView *>(_booksLayout->itemAt(0)->widget())->book()->yearOfPublishing();
    unsigned min = year;
    unsigned max = year;

    for (int i = 1; i < _booksLayout->count(); ++i) {
        year = static_cast<BookView *>(_booksLayout->itemAt(i)->widget())->book()->yearOfPublishing();

        min = year < min ? year : min;
        max = year > max ? year : max;
    }

    _periodLowerSpinBox->setRange(min, max);
    _periodLowerSpinBox->setValue(min);

    _periodHigherSpinBox->setRange(min, max);
    _periodHigherSpinBox->setValue(max);
}

void MainWindow::syncPeriodList() {
    _periodListWidget->clear();

    Book const *book;
    QSet<QString const> uniqueAuthorsSet;
    for (int i = 0; i < this->_booksLayout->count(); ++i) {
        book = static_cast<BookView *>(_booksLayout->itemAt(i)->widget())->book();
        if (static_cast<unsigned>(_periodLowerSpinBox->value()) <= book->yearOfPublishing() &&
            static_cast<unsigned>(_periodHigherSpinBox->value()) >= book->yearOfPublishing() &&
            !uniqueAuthorsSet.contains(book->author()))
        {
            uniqueAuthorsSet << book->author();
            _periodListWidget->addItem(book->author());
        }
    }
}

void MainWindow::syncChronoList(QString const &publishing) {
    _chronoList->clear();

    Book const *book;
    QMultiMap<unsigned, QString> booksByYear;
    for (int i = 0; i < this->_booksLayout->count(); ++i) {
        book = static_cast<BookView *>(_booksLayout->itemAt(i)->widget())->book();
        if (book->publishing() == publishing) {
            booksByYear.insert(book->yearOfPublishing(), book->name());
        }
    }

    foreach (unsigned const year, booksByYear.uniqueKeys()) {
        foreach (QString const& name, booksByYear.values(year)) {
            _chronoList->addItem(tr("Name") + ": " + name + "\n    " +
                                 tr("Year") + ": " + QString::number(year));
        }
    }
}

void MainWindow::syncChartViewTab() {
    if (_tabsWidget->currentWidget() != _chartView)
        return;
    this->syncChart();
}

void MainWindow::syncUniqueBooksTab() {
    if (_tabsWidget->currentWidget() != _uniqueBooksListWidget)
        return;
    this->syncUniqueBooksList();
}

void MainWindow::syncAuthorByPeriodTab() {
    if (_tabsWidget->currentWidget() != _periodWidget)
        return;

    this->syncPeriodSpinBoxes();
    this->syncPeriodList();
}

void MainWindow::syncChronoTab() {
    if (_tabsWidget->currentWidget() != _chronoWidget)
        return;
    syncPublishingComboBox();
}

void MainWindow::removeBook(BookView *bookWidget) {
    bookWidget->setVisible(false);
    _booksLayout->removeWidget(bookWidget);
    delete bookWidget->book();
    delete bookWidget;

    emit this->bookDeleted();
    _tabsWidget->currentWidget()->setFocus();
}

void MainWindow::saveToFile(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::information(this,
            tr("Unable to open file"),
            file.errorString());
        return;
    }

    QDataStream out(&file);
    out.setVersion(SERIALIZE_VERSION);
    for (int i = 0; i < this->_booksLayout->count(); ++i) {
        out << *static_cast<BookView *>(this->_booksLayout->itemAt(i)->widget())->book();
    }
}

void MainWindow::loadFromFile(const QString &fileName) {
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(this,
            tr("Unable to open file"),
            file.errorString());
        return;
    }

    bool invalidBookFound = false;
    QDataStream in(&file);
    in.setVersion(SERIALIZE_VERSION);

    Book* book;
    BookView* bookView;
    while (!in.atEnd()) {
        book = new Book;
        in >> *book;

        bookView = new BookView(book, this);
        if (!bookView->validate()) {
            delete bookView;
            invalidBookFound = true;
        }
        else {
            _booksLayout->addWidget(bookView);
            connect(bookView, &BookView::deleteButtonClicked,
                    this, &MainWindow::removeBook);

            emit this->bookAdded();
        }
    }

    emit this->loadedFromFile();
    if (invalidBookFound) {
        emit this->loadedWithErrors();
    }
    else {
        emit this->loadedWithoutErrors();
    }
}

//void MainWindow::syncChronoList() {
//    if (_tabsWidget->currentWidget() != _chronoList)
//        return;

//    _chronoList->clear();

//    if (_booksLayout->count() == 0)
//        return;

//    Book const *book;
//    QList<Book const *> sortedBookList;
//    for (int i = 0; i < this->_booksLayout->count(); ++i) {
//        book = static_cast<BookView *>(_booksLayout->itemAt(i)->widget())->book();
//        if (book->publishing() == _publishingComboBox->currentText()) {
//            sortedBookList << book;
//            _publishingComboBox->addItem(book->publishing());
//        }
//    }
//    std::sort(sortedBookList.begin(), sortedBookList.end(),
//        [](Book const * left, Book const * right) {
//            return left->yearOfPublishing() < right->yearOfPublishing();
//    });

//    for (auto const &i: sortedBookList) {
//        _chronoList->addItem(i->name() + ' ' + i->yearOfPublishing());
//    }
//}

//void MainWindow::initPeriodTab(){
//    _periodWidget = new QWidget(_tabsWidget);
//    _periodLowerSpinBox = new QSpinBox(_periodWidget);
//    _periodHigherSpinBox = new QSpinBox(_periodWidget);
//    auto periodControlsLayout = new QHBoxLayout(_periodWidget);
//    periodControlsLayout->addWidget(_periodLowerSpinBox);
//    periodControlsLayout->addWidget(_periodHigherSpinBox);

//    _periodListWidget = new QListWidget(_periodWidget);

//    auto periodLayout = new QHBoxLayout(_periodWidget);
//    periodLayout->addLayout(periodControlsLayout);
//    periodLayout->addWidget(_periodListWidget);
//    _periodWidget->setLayout(periodLayout);

//    _tabsWidget->addTab(_periodWidget, "Books by period");
//}
