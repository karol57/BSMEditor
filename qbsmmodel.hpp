#ifndef QBSMMODEL_HPP
#define QBSMMODEL_HPP

#include <map>
#include <vector>
#include <bitset>
#include <array>

#include <Qt>
#include <QObject>
#include <QString>
#include <QVariant>
#include <QModelIndex>
#include <QAbstractListModel>
#include <QByteArray>
#include <QStyledItemDelegate>

#include "BSMFile.det.hpp"

class QBSMDelegate : public QStyledItemDelegate {
    Q_OBJECT
    public:
        explicit QBSMDelegate(QObject * parent = nullptr);
        virtual QWidget * createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const override;
        virtual void setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const override;
        virtual QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const override;
        ~QBSMDelegate();
};

class QBSMModel : public QAbstractListModel {
    Q_OBJECT
    public:
        explicit QBSMModel(QObject *parent = 0);
        ~QBSMModel();

        virtual int columnCount(const QModelIndex & parent = QModelIndex()) const override;
        virtual int rowCount(const QModelIndex & parent = QModelIndex()) const override;
        virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
        virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
        virtual Qt::ItemFlags flags(const QModelIndex & index) const override;
        virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

        bool resetFromRAW(const QByteArray & data);
        QByteArray saveToRAW() const noexcept;
        void clear();

        void addLanguage(BSMLANG lang);

        // Only language columns can be removed
        virtual bool removeColumns(int column, int count, const QModelIndex & parent = QModelIndex()) override;
        bool isLanguageColumn(int column) const noexcept;
        bool hasColumnTranslations(int column) const;
        BSMLANG getColumnLanguage(int column) const;

        virtual bool insertRows(int row, int count, const QModelIndex & parent = QModelIndex()) override;
        virtual bool removeRows(int row, int count, const QModelIndex & parent = QModelIndex()) override;

        bool hasRowTranslations(int row) const;
    private:
        struct Column {
            QString name;
            enum { CL_CAT, CL_ID, CL_LANG } type;
            BSMLANG lang; // Valid only when type == CL_LANG
        };
        struct Field {
            enum { FT_NONE, FT_CAT, FT_ID, FT_STRING } type = FT_NONE;
            int catIdLang;  // Only valid when type != FT_NONE
            QString string; // Field text representation
        };
        std::bitset<256> m_languages;
        std::vector<Column> m_columns;

        class {
            public:
                void resize(size_t width, size_t height) {
                    if (height != m_height) {
                        m_data.resize(height);
                        m_height = height;
                    }

                    if (width != m_width) {
                        for (std::vector<Field> & row : m_data)
                            row.resize(width);
                        m_width = width;
                    }
                }
                void clear() noexcept {
                    m_data.clear();
                    m_width = 0;
                    m_height = 0;
                }

                size_t width() const { return m_width; }
                size_t height() const { return m_height; }

                std::vector<std::vector<Field>> m_data;

                size_t m_width = 0;
                size_t m_height = 0;
        } m_fields;

        void _regen_columns();

        bool getPosFromIndex(const QModelIndex & index, size_t & x, size_t & y) const;

    friend QMetaTypeId<Field>;
    friend QBSMDelegate;
};

Q_DECLARE_METATYPE(QBSMModel::Field)

#endif // QBSMMODEL_HPP