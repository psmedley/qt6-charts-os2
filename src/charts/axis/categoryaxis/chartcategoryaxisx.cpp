/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Charts module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <private/chartcategoryaxisx_p.h>
#include <QtCharts/QCategoryAxis>
#include <QtCharts/QAbstractAxis>
#include <private/chartpresenter_p.h>
#include <private/abstractchartlayout_p.h>
#include <QtWidgets/QGraphicsLayout>
#include <QtCore/QtMath>

QT_BEGIN_NAMESPACE

ChartCategoryAxisX::ChartCategoryAxisX(QCategoryAxis *axis, QGraphicsItem* item)
    : HorizontalAxis(axis, item, true),
      m_axis(axis)
{
    QObject::connect(axis, SIGNAL(categoriesChanged()), this, SLOT(handleCategoriesChanged()));
}

ChartCategoryAxisX::~ChartCategoryAxisX()
{
}

QList<qreal> ChartCategoryAxisX::calculateLayout() const
{
    int tickCount = m_axis->categoriesLabels().count() + 1;
    QList<qreal> points;

    if (tickCount < 2)
        return points;

    const QRectF &gridRect = gridGeometry();
    qreal range  = max() - min();
    if (range > 0) {
        points.resize(tickCount);
        qreal scale = gridRect.width() / range;
        for (int i = 0; i < tickCount; ++i) {
            if (i < tickCount - 1) {
                qreal x = (m_axis->startValue(m_axis->categoriesLabels().at(i)) - min()) * scale + gridRect.left();
                points[i] = x;
            } else {
                qreal x = (m_axis->endValue(m_axis->categoriesLabels().at(i - 1)) - min())  * scale + gridRect.left();
                points[i] = x;
            }
        }
    }

    return points;
}

void ChartCategoryAxisX::updateGeometry()
{
    setLabels(m_axis->categoriesLabels() << QString());
    HorizontalAxis::updateGeometry();
}

QSizeF ChartCategoryAxisX::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
    Q_UNUSED(constraint);

    QSizeF sh;
    QSizeF base = HorizontalAxis::sizeHint(which, constraint);
    QStringList ticksList = m_axis->categoriesLabels();
    qreal width = 0; // Width is irrelevant for X axes with interval labels
    qreal height = 0;

    switch (which) {
    case Qt::MinimumSize: {
        if (labelsVisible()) {
            QRectF boundingRect = ChartPresenter::textBoundingRect(axis()->labelsFont(),
                                                                   QStringLiteral("..."),
                                                                   axis()->labelsAngle());
            height = boundingRect.height() + labelPadding() + base.height() + 1.0;
        } else {
            height = base.height() + 1.0;
        }
        sh = QSizeF(width, height);
        break;
    }
    case Qt::PreferredSize: {
        if (labelsVisible()) {
            qreal labelHeight = 0.0;
            foreach (const QString& s, ticksList) {
                QRectF rect = ChartPresenter::textBoundingRect(axis()->labelsFont(), s, axis()->labelsAngle());
                labelHeight = qMax(rect.height(), labelHeight);
            }
            height = labelHeight + labelPadding() + base.height() + 1.0;
        } else {
            height = base.height() + 1.0;
        }
        sh = QSizeF(width, height);
        break;
    }
    default:
        break;
    }

    return sh;
}

void ChartCategoryAxisX::handleCategoriesChanged()
{
    QGraphicsLayoutItem::updateGeometry();
    presenter()->layout()->invalidate();
}

QT_END_NAMESPACE

#include "moc_chartcategoryaxisx_p.cpp"
