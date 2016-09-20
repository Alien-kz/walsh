#ifndef WALSHTRANFORM_H
#define WALSHTRANFORM_H

#include <QString>
#include <QTextEdit>
#include <QImage>

class WalshTransform
{

    int** newMatrix(int n, int ** z);
    void deleteMatrix(int n, int ** z);
    int align_down(int n);
    int align_up(int n);
    void generate(int n, int **w);  //generate system of n Walsh function (size = n)

    int size, size_aligned;
    int **walsh; //w[i][j] == w_i(x), where j < x < j + 1
    int **image;
    int **spectr;
    int **spectr2d;
    int **imageRestored;

    void print(int n, int **w);     //print Matrix
    void makeFourierWalshInRow(int n, int **w, int **signal, int **spectr);     //make transform row by row
    void makeFourierWalshInCol(int n, int **w, int **signal, int **spectr);     //make transform column by colomn

    QTextEdit *log;
    void doLog(QString s);
    void clearAll();

    int power2round(int x);

public:
    WalshTransform();
    ~WalshTransform();
    int load(QString fname);
    int load(QImage *img);
    QImage* txtToPixmap(QImage *img, int ARRAY);

    void transform();
    void retransform();
    void setLog(QTextEdit* textEdit);

    void filter(int level, int filterType);

    enum { NO, HYPER, CROSSDOWN, CROSSUP};
    enum { IMAGE_ORIGINAL, SPECTR, IMAGE_SPECTR2D, IMAGE_RESTORED };
};


#endif // WALSHTRANFORM_H
