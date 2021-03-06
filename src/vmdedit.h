#ifndef VMDEDIT_H
#define VMDEDIT_H

#include "vedit.h"
#include <QVector>
#include <QString>
#include <QColor>
#include <QClipboard>
#include <QImage>
#include "vtoc.h"
#include "veditoperations.h"
#include "vconfigmanager.h"
#include "utils/vutils.h"

class HGMarkdownHighlighter;
class VCodeBlockHighlightHelper;
class VDocument;
class VImagePreviewer;

class VMdEdit : public VEdit
{
    Q_OBJECT
public:
    VMdEdit(VFile *p_file, VDocument *p_vdoc, MarkdownConverterType p_type,
            QWidget *p_parent = 0);
    void beginEdit() Q_DECL_OVERRIDE;
    void endEdit() Q_DECL_OVERRIDE;
    void saveFile() Q_DECL_OVERRIDE;
    void reloadFile() Q_DECL_OVERRIDE;

    // An image has been inserted. The image is relative.
    // @p_path is the absolute path of the inserted image.
    void imageInserted(const QString &p_path);

    void scrollToHeader(const VAnchor &p_anchor);

    // Like toPlainText(), but remove image preview characters.
    QString toPlainTextWithoutImg();

    const QVector<VHeader> &getHeaders() const;

public slots:
    bool jumpTitle(bool p_forward, int p_relativeLevel, int p_repeat) Q_DECL_OVERRIDE;

signals:
    void headersChanged(const QVector<VHeader> &headers);

    // Signal when current header change.
    void curHeaderChanged(VAnchor p_anchor);

    // Signal when the status of VMdEdit changed.
    // Will be emitted by VImagePreviewer for now.
    void statusChanged();

private slots:
    void updateOutline(const QVector<VElementRegion> &p_headerRegions);

    // When there is no header in current cursor, will signal an invalid header.
    void updateCurHeader();

    void handleClipboardChanged(QClipboard::Mode p_mode);

protected:
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    bool canInsertFromMimeData(const QMimeData *source) const Q_DECL_OVERRIDE;
    void insertFromMimeData(const QMimeData *source) Q_DECL_OVERRIDE;
    void updateFontAndPalette() Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *p_event) Q_DECL_OVERRIDE;

private:
    struct Region
    {
        Region() : m_startPos(-1), m_endPos(-1)
        {
        }

        Region(int p_start, int p_end)
            : m_startPos(p_start), m_endPos(p_end)
        {
        }

        int m_startPos;
        int m_endPos;
    };

    // Get the initial images from file before edit.
    void initInitImages();

    // Clear two kind of images according to initial images and current images:
    // 1. Newly inserted images which are deleted later;
    // 2. Initial images which are deleted;
    void clearUnusedImages();

    // There is a QChar::ObjectReplacementCharacter (and maybe some spaces)
    // in the selection. Get the QImage.
    QImage tryGetSelectedImage();

    // Return the header index in m_headers where current cursor locates.
    int currentCursorHeader() const;

    QString getPlainTextWithoutPreviewImage() const;

    // Try to get all the regions of preview image within @p_block.
    // Returns false if preview image is not ready yet.
    bool getPreviewImageRegionOfBlock(const QTextBlock &p_block,
                                      QVector<Region> &p_regions) const;

    void finishOneAsyncJob(int p_idx);

    HGMarkdownHighlighter *m_mdHighlighter;
    VCodeBlockHighlightHelper *m_cbHighlighter;
    VImagePreviewer *m_imagePreviewer;

    // Image links inserted while editing.
    QVector<ImageLink> m_insertedImages;

    // Image links right at the beginning of the edit.
    QVector<ImageLink> m_initImages;

    QVector<VHeader> m_headers;

    bool m_freshEdit;

    QVector<bool> m_finishedAsyncJobs;

    static const int c_numberOfAysncJobs;
};

#endif // VMDEDIT_H
