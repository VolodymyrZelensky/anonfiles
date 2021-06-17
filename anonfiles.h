#ifndef ANONFILES_H
#define ANONFILES_H

#include <QMainWindow>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QDesktopServices>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QHttpMultiPart>
#include <QHttpPart>
#include <QUrl>
#include <QUrlQuery>
#include <QDir>
#include <QFile>
#include <QFont>
#include <QFontDatabase>
#include <QFileInfo>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include "sql/qdb.h"

QT_BEGIN_NAMESPACE
namespace Ui { class anonfiles; }
QT_END_NAMESPACE

class anonfiles : public QMainWindow
{
		Q_OBJECT

	public:
		anonfiles(QWidget *parent = nullptr);
		~anonfiles();

	private slots:
		void UploadDone(QNetworkReply *reply);
		void UploadProgress(qint64 received, qint64 total);
		void on_btn_select_clicked();
		void on_btn_upload_clicked();
		bool AreYouOnline();
		void on_btn_twitter_clicked();
		void on_btn_copy_fd_clicked();
		void on_btn_copy_sd_clicked();
		void on_btn_copy_fn_clicked();
		void on_btn_copy_fi_clicked();

	private:
		Ui::anonfiles *ui;
		QDBLite::DB db;

};
#endif // ANONFILES_H
