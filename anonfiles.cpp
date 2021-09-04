#include "anonfiles.h"
#include "ui_anonfiles.h"

anonfiles::anonfiles(QWidget *parent): QMainWindow(parent), ui(new Ui::anonfiles){
	ui->setupUi(this);
	QFontDatabase::addApplicationFont(":/fonts/TitilliumWeb.ttf");
	ui->btn_select->setFont(QFont("Titillium Web", 10, QFont::Normal));
	ui->btn_upload->setFont(QFont("Titillium Web", 10, QFont::Normal));
	ui->FID->setFont(QFont("Titillium Web", 9, QFont::Normal));
	ui->Slink->setFont(QFont("Titillium Web", 9, QFont::Normal));
	ui->Flink->setFont(QFont("Titillium Web", 9, QFont::Normal));
	ui->Fname->setFont(QFont("Titillium Web", 9, QFont::Normal));
	ui->file_path->setFont(QFont("Titillium Web", 9, QFont::Normal));
	ui->upload_progress->setFont(QFont("Titillium Web", 9, QFont::Normal));
	if(!QDir(QDir::homePath() + "/.local/share/anonfiles").exists()){
		QDir qDir;
		qDir.mkdir(QDir::homePath() + "/.local/share/anonfiles");
	}
	if(!QFile::exists(QDir::homePath() + "/.local/share/anonfiles/Database.db")){
		QFile::copy(":/database/Database.db", QDir::homePath() + "/.local/share/anonfiles/Database.db");
		QFile(QDir::homePath() + "/.local/share/anonfiles/Database.db").setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner);
	}
	db.dbstate = db.Connect(QDir::homePath() + "/.local/share/anonfiles/Database.db");
}

anonfiles::~anonfiles(){delete ui;}
void anonfiles::on_btn_twitter_clicked()
{
	QDesktopServices::openUrl(QUrl("https://twitter.com/_Lil_Nickel"));
}
bool anonfiles::AreYouOnline(){
    QTcpSocket* sock = new QTcpSocket(this);
	sock->connectToHost("google.com", 80);
    bool connected = sock->waitForConnected(30000);//ms
    if (!connected){
        sock->abort();
        return false;
    }
    sock->close();
    return true;
}
void anonfiles::on_btn_copy_fd_clicked(){
	ui->Flink->selectAll();
	ui->Flink->copy();
	ui->Flink->deselect();
}
void anonfiles::on_btn_copy_sd_clicked(){
	ui->Slink->selectAll();
	ui->Slink->copy();
	ui->Slink->deselect();
}
void anonfiles::on_btn_copy_fn_clicked(){
	ui->Fname->selectAll();
	ui->Fname->copy();
	ui->Fname->deselect();
}
void anonfiles::on_btn_copy_fi_clicked(){
	ui->FID->selectAll();
	ui->FID->copy();
	ui->FID->deselect();
}
void anonfiles::on_btn_select_clicked()
{
	QString FilePath = QFileDialog::getOpenFileName(this,tr("Select File"), QDir::homePath());
	if(FilePath != NULL){
		if((QFileInfo(FilePath).size()/1024000)>=5000){
			QMessageBox messageBox;
			messageBox.setIcon(QMessageBox::Information);
			messageBox.setStyleSheet("QMessageBox{background-color:#1e2228;}"
									 "QMessageBox QLabel{font:9pt \"Titillium Web\";}"
									 "QMessageBox QPushButton{background-color:#373e47;border:1px solid #444c56;border-radius:6;padding-left:13px;padding-right:13px;font: 9pt \"Titillium Web\";padding-bottom: 2px;}"
									 "QMessageBox QPushButton:hover{background-color:#444c56;border-color:#768390;}"
									 "QMessageBox QPushButton:pressed{background-color:#373e47;}"
									 "*{selection-background-color:rgba( 255, 255, 255, 0% );selection-color:#539bf5;color:#adbac7;}");
			messageBox.setWindowIcon(QIcon(":/images/assets/logo-24x24.ico"));
			messageBox.setWindowTitle("ERROR_FILE_SIZE_EXCEEDED");
			messageBox.setText("The file is too large. Max filesize: 5 GiB");
			messageBox.exec();
			return;
		}
		ui->btn_upload->setEnabled(true);
		ui->file_path->setText(FilePath);
	}
}

void anonfiles::on_btn_upload_clicked()
{
	if(!this->AreYouOnline()){
		QMessageBox messageBox;
		messageBox.setIcon(QMessageBox::Warning);
		messageBox.setStyleSheet("QMessageBox{background-color:#1e2228;}"
								 "QMessageBox QLabel{font:9pt \"Titillium Web\";}"
								 "QMessageBox QPushButton{background-color:#373e47;border:1px solid #444c56;border-radius:6;padding-left:13px;padding-right:13px;font: 9pt \"Titillium Web\";padding-bottom: 2px;}"
								 "QMessageBox QPushButton:hover{background-color:#444c56;border-color:#768390;}"
								 "QMessageBox QPushButton:pressed{background-color:#373e47;}"
								 "*{selection-background-color:rgba( 255, 255, 255, 0% );selection-color:#539bf5;color:#adbac7;}");
		messageBox.setWindowIcon(QIcon(":/images/assets/logo-24x24.ico"));
		messageBox.setWindowTitle("Network Connection");
		messageBox.setText("please connect network and try again...");
		messageBox.exec();
		return;
	}
	ui->upload_progress->setValue(0);
	ui->btn_select->setEnabled(false);
	ui->btn_upload->setEnabled(false);
	QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
	QHttpPart SFile;
	SFile.setHeader(
		QNetworkRequest::ContentDispositionHeader,
		QVariant("form-data; name=\"file\"; filename=\"" + QFileInfo(ui->file_path->text()).fileName() + "\"")
	);
	QFile *file = new QFile(ui->file_path->text());
	file->open(QIODevice::ReadOnly);
	SFile.setBodyDevice(file);
	file->setParent(multiPart);
	multiPart->append(SFile);
	QUrl url("https://api.anonfiles.com/upload");
	QNetworkRequest request(url);
	QNetworkAccessManager *manager = new QNetworkAccessManager(this);
	QNetworkReply *reply = manager->post(request, multiPart);
	connect(manager, &QNetworkAccessManager::finished, this, &anonfiles::UploadDone);
	connect(reply, SIGNAL(uploadProgress(qint64, qint64)), this, SLOT(UploadProgress(qint64, qint64)));
	multiPart->setParent(reply);
}

void anonfiles::UploadDone(QNetworkReply *reply){
	ui->btn_select->setEnabled(true);
	ui->btn_upload->setEnabled(true);
	QByteArray response = reply->readAll();
	QJsonDocument doc = QJsonDocument::fromJson(response);
	QJsonObject JON = doc.object();
	if(JON.value("status").toBool()){
		ui->Flink->setText(JON.value("data").toObject().value("file").toObject().value("url").toObject().value("full").toString());
		ui->Slink->setText(JON.value("data").toObject().value("file").toObject().value("url").toObject().value("short").toString());
		ui->FID->setText(JON.value("data").toObject().value("file").toObject().value("metadata").toObject().value("id").toString());
		ui->Fname->setText(JON.value("data").toObject().value("file").toObject().value("metadata").toObject().value("name").toString());
		QSqlQuery add(db.db);
		add.prepare("INSERT INTO uploads (id, name, link) VALUES (:a, :b, :c)");
		add.bindValue(":a", JON.value("data").toObject().value("file").toObject().value("metadata").toObject().value("id").toString());
		add.bindValue(":b", JON.value("data").toObject().value("file").toObject().value("metadata").toObject().value("name").toString());
		add.bindValue(":c", JON.value("data").toObject().value("file").toObject().value("url").toObject().value("short").toString());
		add.exec();
		ui->file_path->setText("");
		ui->btn_upload->setEnabled(false);
		ui->upload_progress->setValue(0);
	}else{
		QMessageBox messageBox;
		messageBox.setIcon(QMessageBox::Warning);
		messageBox.setStyleSheet("QMessageBox{background-color:#1e2228;}"
								 "QMessageBox QLabel{font:9pt \"Titillium Web\";}"
								 "QMessageBox QPushButton{background-color:#373e47;border:1px solid #444c56;border-radius:6;padding-left:13px;padding-right:13px;font: 9pt \"Titillium Web\";padding-bottom: 2px;}"
								 "QMessageBox QPushButton:hover{background-color:#444c56;border-color:#768390;}"
								 "QMessageBox QPushButton:pressed{background-color:#373e47;}"
								 "*{selection-background-color:rgba( 255, 255, 255, 0% );selection-color:#539bf5;color:#adbac7;}");
		messageBox.setWindowIcon(QIcon(":/images/assets/logo-24x24.ico"));
		QString Err_txt = JON.value("error").toObject().value("message").toString();
		QString Err_cod = JON.value("error").toObject().value("type").toString();
		if(Err_txt == NULL || Err_txt == nullptr) Err_txt = "Unknown Error From anonfiles Server ...";
		if(Err_cod == NULL || Err_cod == nullptr) Err_cod = "Unknown";
		messageBox.setWindowTitle(Err_cod);
		messageBox.setText(Err_txt);
		messageBox.exec();
		ui->upload_progress->setValue(0);
	}
}

void anonfiles::UploadProgress(qint64 received, qint64 total){
	if(received != 0 && total != 0){
		qint64 percent;
		percent = received * 100 / total;
		ui->upload_progress->setValue(percent);
	}
}
