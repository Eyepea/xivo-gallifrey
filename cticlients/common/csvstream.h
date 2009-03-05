#ifndef __CSVSTREAM_H__
#define __CSVSTREAM_H__

#include <QTextStream>
#include <QStringList>

/*! \brief Stream used to read CSV files
 */
class CsvStream : public QTextStream
{
public:
    CsvStream(QIODevice * device);
    QStringList readRecords();
private:
    QChar fieldSeparator;   //!< separator between fields (coma by default)
    QChar textDelimiter;    //!< string delimiter (double quote by default)
};

#endif

