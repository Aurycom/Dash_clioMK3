#include "DialogAutoResize.hpp"

DialogAutoResize::DialogAutoResize(Arbiter &arbiter, bool fullscreen, QWidget *parent)
    : QDialog(parent, Qt::FramelessWindowHint)
    , arbiter(arbiter)
{
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    this->setStyleSheet("border-radius:8px;");
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    
    this->fullscreen = fullscreen;
    if (this->fullscreen)
        this->setModal(true);

    this->mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    //mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    //mainLayout->addStretch();
    mainLayout->addWidget(this->content_widget());
    mainLayout->addStretch();
    this->setLayout(mainLayout);

    this->timer = new QTimer(this);
    this->timer->setSingleShot(true);
    connect(this->timer, &QTimer::timeout, [this]() { this->close(); });

    this->installEventFilter(this);
}

void DialogAutoResize::open(int timeout)
{
    this->show();
    this->raise();
    this->activateWindow();
    if (timeout > 0)
        this->timer->start(timeout);
}

void DialogAutoResize::set_title(QString str)
{
    QLabel *label = new QLabel(str, this);
    QFont font(this->arbiter.forge().font(16));
    font.setBold(true);
    label->setFont(font);
    //label->setStyleSheet("border:1px solid red;");
    this->title->addWidget(label);
}

void DialogAutoResize::set_body(QWidget *widget)
{
    /*if (this->fullscreen) {
        QScrollArea *scroll_area = new QScrollArea(this);
        Session::Forge::to_touch_scroller(scroll_area);
        scroll_area->setWidgetResizable(true);
	scroll_area->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
        scroll_area->setWidget(widget);

        this->body->addWidget(scroll_area);
    }
    else {*/
	//widget->addStretch();
	//widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        this->body->addWidget(widget);
    //}
}

void DialogAutoResize::resize(){
	//this->adjustSize();
	this->title->invalidate();
        this->title->activate();
        this->title->update();

	this->body->invalidate();
	this->body->activate();
	this->body->update();

	this->frame->adjustSize();
	this->frame->updateGeometry();
	//this->set_position();

	this->mainLayout->invalidate();
	this->mainLayout->activate();
	this->mainLayout->update();

	this->adjustSize();
	this->updateGeometry();
	//this->set_position();

	this->hide();
	this->show();
	this->update();
	//QMargins marg = QMargins(20,20,20,240);

	//QRect rect = QRect(0,0, this->width(), this->height());
	//rect.marginsAdded(marg);

	/*this->clearMask();
        QPainterPath path;
        path.addRoundedRect(this->rect(), 8, 8);
        this->setMask(path.toFillPolygon().toPolygon());*/
}

void DialogAutoResize::set_button(QPushButton *button)
{
    /*if (this->buttons->count() == 0)
        this->add_cancel_button();*/
    button->setFlat(true);
    this->buttons->addWidget(button, 0, Qt::AlignRight);
    connect(button, &QPushButton::clicked, [this]() { this->close(); });
}

QWidget *DialogAutoResize::content_widget()
{
    this->frame = new QFrame(this);
    this->frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred); 
    //this->frame->setStyleSheet("border-radius:8px;");
    //this->frame->setStyleSheet("border:1px solid blue;");
    QVBoxLayout *layout = new QVBoxLayout(this->frame);
    layout->setContentsMargins(20, 10, 20, 10);
    layout->setSpacing(0);
    //layout->addStretch();
    //this->frame->setLayout(layout);

    this->title = new QVBoxLayout();
    this->title->setContentsMargins(0, 10, 0, 10);
    this->title->setSpacing(0);
    this->title->setAlignment(Qt::AlignCenter);
    //this->title->setSizeConstraint(QLayout::SetFixedSize);
    //this->title->addStretch();
    //this->title->setStyleSheet("border:1px solid blue;");
    layout->addLayout(this->title);

    this->body = new QVBoxLayout();
    this->body->setContentsMargins(0, 0, 0, 0);
    //this->body->setSizeConstraint(QLayout::SetFixedSize);
    //this->body->addStretch();
    this->body->setAlignment(Qt::AlignTop);
    //this->body->setStyleSheet("border:1px solid blue;");
    this->body->setSpacing(0);
    layout->addLayout(this->body);

    /*this->buttons = new QHBoxLayout();
    this->buttons->setContentsMargins(0, 0, 0, 0);
    this->buttons->setSpacing(0);
    this->buttons->addStretch();
    if (this->fullscreen)
        this->add_cancel_button();
    layout->addLayout(this->buttons);*/

    return frame;
}

void DialogAutoResize::set_position()
{
    if (QWidget *parent = this->parentWidget()) {
        QPoint point;
        if (this->fullscreen) {
            point = parent->geometry().center() - this->rect().center();
        }
        else {
            QWidget *window = parent->window();
            QPoint window_center = window->mapToGlobal(window->rect().center());
            QPoint parent_center = parent->mapToGlobal(parent->rect().center());

            int offset = std::ceil(4 * this->arbiter.layout().scale);

            QPoint pivot;
            if (parent_center.y() > window_center.y()) {
                pivot = (parent_center.x() > window_center.x()) ? this->rect().bottomRight() : this->rect().bottomLeft();
                pivot.ry() += (parent->height() / 2) + offset;
            }
            else {
                pivot = (parent_center.x() > window_center.x()) ? this->rect().topRight() : this->rect().topLeft();
                pivot.ry() -= (parent->height() / 2) + offset;
            }
            if (parent_center.x() > window_center.x())
                pivot.rx() -= this->width() / 2;
            else
                pivot.rx() += this->width() / 2;
            point = this->mapFromGlobal(parent_center) - pivot;
        }
        this->move(point);
    }
}

void DialogAutoResize::keyPressEvent(QKeyEvent *event)
{
    if (event->key() != Qt::Key_Escape || this->fullscreen)
        QDialog::keyPressEvent(event);
}

void DialogAutoResize::showEvent(QShowEvent *event)
{
    // set to null position
    this->move(QPoint());
    QWidget::showEvent(event);

    if (this->fullscreen) {
        if (QWidget *parent = this->parentWidget()) {
            /*int margin = std::ceil(48 * this->arbiter.layout().scale) * 2;
            this->setFixedWidth(std::min(this->width(), parent->width() - margin));
            this->setFixedHeight(std::min(this->height(), parent->height() - margin));*/
        }
    }

    this->set_position();
/*	this->clearMask();
	ClioMK3DIALOG_LOG(info)<<"taille "<<this->height();
	ClioMK3DIALOG_LOG(info)<<"taille2 "<<this->rect().height();
	ClioMK3DIALOG_LOG(info)<<"taille2 "<<this->frameGeometry().height();
	QRect rect = QRect(0,0, this->frameGeometry().width(), this->frameGeometry().height());
	QPainterPath path;
        path.addRoundedRect(rect, 8, 8);
        this->setMask(path.toFillPolygon().toPolygon());*/


	QTimer::singleShot(0,this, &DialogAutoResize::set_mask);



}

void DialogAutoResize::set_mask(){
	this->clearMask();
	//Rect rect = QRect(0,0, this->frameGeometry().width(), this->frameGeometry().height());
        QPainterPath path;
        path.addRoundedRect(this->rect(), 8, 8);
        this->setMask(path.toFillPolygon().toPolygon());
}

void DialogAutoResize::closeEvent(QCloseEvent *)
{
    // On Raspberry Pi's, sometimes LXPanel will grab focus after a dialog closes
    // Focus should be returned to main dash window instead, so that shortcuts work
    // and that dash remains fullscreen
    //
    // Tells the main window to grab focus
    this->arbiter.window()->activateWindow();
}

bool DialogAutoResize::eventFilter(QObject *object, QEvent *event)
{
    // restart timer on any event
    if (this->timer->isActive())
        this->timer->start(this->timer->interval());

    return QWidget::eventFilter(object, event);
}

/*SnackBar::SnackBar(Arbiter &arbiter)
    : Dialog(arbiter, false, this->get_ref())
{
    this->setFixedHeight(64 * this->arbiter.layout().scale);
}

void SnackBar::resizeEvent(QResizeEvent* event)
{
    // its possible the ref didnt exist when the parent was originally set
    if (!this->parentWidget()) {
        auto flags = this->windowFlags();
        this->setParent(this->get_ref());
        this->setWindowFlags(flags);
    }

    if (QWidget *parent = this->parentWidget())
        this->setFixedWidth(parent->width() * (2 / 3.0));

    DialogAutoResize::resizeEvent(event);
}

void SnackBar::mousePressEvent(QMouseEvent *event)
{
    this->close();
}

QWidget *SnackBar::get_ref()
{
    for (QWidget *widget : qApp->allWidgets()) {
        if (widget->objectName() == "MsgRef")
            return widget;
    }
    return nullptr;
}*/
