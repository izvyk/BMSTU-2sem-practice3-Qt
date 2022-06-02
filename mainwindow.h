#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "BookView.h"

#include <QMainWindow>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QChartView>
#include <QBarSeries>
#include <QValueAxis>
#include <QBarCategoryAxis>
#include <QListWidget>
#include <QComboBox>
#include <QSpinBox>


class MainWindow: public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget* parent = nullptr);

private:
    bool _addingBook;
    QVBoxLayout *_booksLayout;
    QTabWidget *_tabsWidget;
    QPushButton *_saveButton;
    QPushButton *_loadButton;
    QPushButton *_addBookButton;

    QtCharts::QChartView *_chartView;
    QtCharts::QBarCategoryAxis *_axisX;
    QtCharts::QValueAxis *_axisY;
    QtCharts::QBarSeries *_series;

    QListWidget *_uniqueBooksListWidget;

    QWidget *_periodWidget;
    QListWidget *_periodListWidget;
    QSpinBox *_periodLowerSpinBox;
    QSpinBox *_periodHigherSpinBox;

    QWidget *_chronoWidget;
    QListWidget *_chronoList;
    QComboBox *_publishingComboBox;

    void initBookListTab();
    void initUniqueBooksTab();
    void initChartViewTab();
    void initBookListButtons();
    void initPeriodTab();
    void initChronoTab();

private slots:
    void syncSaveButtonClickability() {
        _saveButton->setEnabled(!_addingBook && _booksLayout->count() > 0);
    }

    void syncChartTabClickability() {
        _tabsWidget->setTabEnabled(_tabsWidget->indexOf(_chartView), !_addingBook && _booksLayout->count() > 0);
    }

    void syncUniqueBooksTabClickability() {
        _tabsWidget->setTabEnabled(_tabsWidget->indexOf(_uniqueBooksListWidget), !_addingBook && _booksLayout->count() > 0);
    }

    void syncPeriodTabClickability() {
        _tabsWidget->setTabEnabled(_tabsWidget->indexOf(_periodWidget), !_addingBook && _booksLayout->count() > 0);
    }

    void syncChronoTabClickability() {
        _tabsWidget->setTabEnabled(_tabsWidget->indexOf(_chronoWidget), !_addingBook && _booksLayout->count() > 0);
    }

    void syncLoadButtonClickability() {
        _loadButton->setEnabled(!_addingBook);
    }

    void syncAddBookButtonClickability() {
        _addBookButton->setEnabled(!_addingBook);
    }

    void syncChart();

    void syncUniqueBooksList();

    void syncPublishingComboBox();

    void syncPeriodSpinBoxes();

    void syncPeriodList();

    void syncChronoList(QString const &);

    void syncAllControlsClickability() {
        this->syncChartTabClickability();
        this->syncAddBookButtonClickability();
        this->syncSaveButtonClickability();
        this->syncLoadButtonClickability();
        this->syncChartTabClickability();
        this->syncUniqueBooksTabClickability();
        this->syncPeriodTabClickability();
        this->syncChronoTabClickability();
    }

    void syncChartViewTab();

    void syncUniqueBooksTab();

    void syncAuthorByPeriodTab();

    void syncChronoTab();

    void syncAllTabs() {
        this->syncChartViewTab();
        this->syncUniqueBooksTab();
        this->syncAuthorByPeriodTab();
        this->syncChronoTab();
    }

public slots:
    void removeBook(BookView * bookWidget);

    void saveToFile(QString const& name);

    void loadFromFile(QString const& name);

signals:
    void bookDeleted();

    void bookAdded();

    void loadedFromFile();

    void loadedWithoutErrors();

    void loadedWithErrors();

    void addBookStarted();

    void addBookFinished();

};
#endif // MAINWINDOW_H
