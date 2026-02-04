# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'preferencesdialog.ui'
##
## Created by: Qt User Interface Compiler version 6.10.1
##
## WARNING! All changes made in this file will be lost when recompiling UI file!
################################################################################

from PySide6.QtCore import (QCoreApplication, QDate, QDateTime, QLocale,
    QMetaObject, QObject, QPoint, QRect,
    QSize, QTime, QUrl, Qt)
from PySide6.QtGui import (QBrush, QColor, QConicalGradient, QCursor,
    QFont, QFontDatabase, QGradient, QIcon,
    QImage, QKeySequence, QLinearGradient, QPainter,
    QPalette, QPixmap, QRadialGradient, QTransform)
from PySide6.QtWidgets import (QAbstractButton, QApplication, QCheckBox, QComboBox,
    QDialog, QDialogButtonBox, QFormLayout, QGroupBox,
    QHBoxLayout, QLabel, QPushButton, QSizePolicy,
    QSpacerItem, QSpinBox, QTabWidget, QVBoxLayout,
    QWidget)

class Ui_PreferencesDialog(object):
    def setupUi(self, PreferencesDialog):
        if not PreferencesDialog.objectName():
            PreferencesDialog.setObjectName(u"PreferencesDialog")
        PreferencesDialog.resize(548, 551)
        self.verticalLayout_7 = QVBoxLayout(PreferencesDialog)
        self.verticalLayout_7.setObjectName(u"verticalLayout_7")
        self.tabWidget = QTabWidget(PreferencesDialog)
        self.tabWidget.setObjectName(u"tabWidget")
        self.General = QWidget()
        self.General.setObjectName(u"General")
        self.verticalLayout_9 = QVBoxLayout(self.General)
        self.verticalLayout_9.setObjectName(u"verticalLayout_9")
        self.groupBox = QGroupBox(self.General)
        self.groupBox.setObjectName(u"groupBox")
        self.verticalLayout_3 = QVBoxLayout(self.groupBox)
        self.verticalLayout_3.setObjectName(u"verticalLayout_3")
        self.clockCheckBox = QCheckBox(self.groupBox)
        self.clockCheckBox.setObjectName(u"clockCheckBox")

        self.verticalLayout_3.addWidget(self.clockCheckBox)

        self.menuBarCheckBox = QCheckBox(self.groupBox)
        self.menuBarCheckBox.setObjectName(u"menuBarCheckBox")

        self.verticalLayout_3.addWidget(self.menuBarCheckBox)

        self.unbreakableSpaceCheckBox = QCheckBox(self.groupBox)
        self.unbreakableSpaceCheckBox.setObjectName(u"unbreakableSpaceCheckBox")

        self.verticalLayout_3.addWidget(self.unbreakableSpaceCheckBox)

        self.horizontalLayout = QHBoxLayout()
        self.horizontalLayout.setObjectName(u"horizontalLayout")
        self.label = QLabel(self.groupBox)
        self.label.setObjectName(u"label")

        self.horizontalLayout.addWidget(self.label)

        self.cornersComboBox = QComboBox(self.groupBox)
        self.cornersComboBox.setObjectName(u"cornersComboBox")

        self.horizontalLayout.addWidget(self.cornersComboBox)

        self.horizontalLayout.setStretch(1, 1)

        self.verticalLayout_3.addLayout(self.horizontalLayout)

        self.verticalSpacer_3 = QSpacerItem(20, 18, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.verticalLayout_3.addItem(self.verticalSpacer_3)


        self.verticalLayout_9.addWidget(self.groupBox)

        self.groupBox_2 = QGroupBox(self.General)
        self.groupBox_2.setObjectName(u"groupBox_2")
        self.verticalLayout_2 = QVBoxLayout(self.groupBox_2)
        self.verticalLayout_2.setObjectName(u"verticalLayout_2")
        self.autoSaveCheckBox = QCheckBox(self.groupBox_2)
        self.autoSaveCheckBox.setObjectName(u"autoSaveCheckBox")

        self.verticalLayout_2.addWidget(self.autoSaveCheckBox)

        self.backupCheckBox = QCheckBox(self.groupBox_2)
        self.backupCheckBox.setObjectName(u"backupCheckBox")

        self.verticalLayout_2.addWidget(self.backupCheckBox)

        self.openDraftDirButton = QPushButton(self.groupBox_2)
        self.openDraftDirButton.setObjectName(u"openDraftDirButton")

        self.verticalLayout_2.addWidget(self.openDraftDirButton)

        self.currentBackupFolderText = QLabel(self.groupBox_2)
        self.currentBackupFolderText.setObjectName(u"currentBackupFolderText")

        self.verticalLayout_2.addWidget(self.currentBackupFolderText)

        self.selectBackupFolderButton = QPushButton(self.groupBox_2)
        self.selectBackupFolderButton.setObjectName(u"selectBackupFolderButton")

        self.verticalLayout_2.addWidget(self.selectBackupFolderButton)

        self.verticalSpacer_2 = QSpacerItem(20, 40, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.verticalLayout_2.addItem(self.verticalSpacer_2)


        self.verticalLayout_9.addWidget(self.groupBox_2)

        self.groupBox_3 = QGroupBox(self.General)
        self.groupBox_3.setObjectName(u"groupBox_3")
        self.verticalLayout = QVBoxLayout(self.groupBox_3)
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.rememberHistoryCheckBox = QCheckBox(self.groupBox_3)
        self.rememberHistoryCheckBox.setObjectName(u"rememberHistoryCheckBox")

        self.verticalLayout.addWidget(self.rememberHistoryCheckBox)

        self.restoreSessionCheckBox = QCheckBox(self.groupBox_3)
        self.restoreSessionCheckBox.setObjectName(u"restoreSessionCheckBox")

        self.verticalLayout.addWidget(self.restoreSessionCheckBox)

        self.verticalSpacer_7 = QSpacerItem(20, 40, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.verticalLayout.addItem(self.verticalSpacer_7)


        self.verticalLayout_9.addWidget(self.groupBox_3)

        self.groupBox_7 = QGroupBox(self.General)
        self.groupBox_7.setObjectName(u"groupBox_7")
        self.verticalLayout1 = QVBoxLayout(self.groupBox_7)
        self.verticalLayout1.setObjectName(u"verticalLayout1")
        self.folderViewShowAllFilesCheckBox = QCheckBox(self.groupBox_7)
        self.folderViewShowAllFilesCheckBox.setObjectName(u"folderViewShowAllFilesCheckBox")

        self.verticalLayout1.addWidget(self.folderViewShowAllFilesCheckBox)

        self.verticalSpacer = QSpacerItem(20, 40, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.verticalLayout1.addItem(self.verticalSpacer)


        self.verticalLayout_9.addWidget(self.groupBox_7)

        self.tabWidget.addTab(self.General, "")
        self.Editor = QWidget()
        self.Editor.setObjectName(u"Editor")
        self.verticalLayout_8 = QVBoxLayout(self.Editor)
        self.verticalLayout_8.setObjectName(u"verticalLayout_8")
        self.groupBox_4 = QGroupBox(self.Editor)
        self.groupBox_4.setObjectName(u"groupBox_4")
        self.verticalLayout_6 = QVBoxLayout(self.groupBox_4)
        self.verticalLayout_6.setObjectName(u"verticalLayout_6")
        self.spacesCheckBox = QCheckBox(self.groupBox_4)
        self.spacesCheckBox.setObjectName(u"spacesCheckBox")

        self.verticalLayout_6.addWidget(self.spacesCheckBox)

        self.horizontalLayout_2 = QHBoxLayout()
        self.horizontalLayout_2.setObjectName(u"horizontalLayout_2")
        self.label_6 = QLabel(self.groupBox_4)
        self.label_6.setObjectName(u"label_6")

        self.horizontalLayout_2.addWidget(self.label_6)

        self.tabWidthInput = QSpinBox(self.groupBox_4)
        self.tabWidthInput.setObjectName(u"tabWidthInput")
        self.tabWidthInput.setMinimum(1)
        self.tabWidthInput.setMaximum(8)

        self.horizontalLayout_2.addWidget(self.tabWidthInput)

        self.horizontalLayout_2.setStretch(1, 1)

        self.verticalLayout_6.addLayout(self.horizontalLayout_2)

        self.verticalSpacer_6 = QSpacerItem(20, 40, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.verticalLayout_6.addItem(self.verticalSpacer_6)


        self.verticalLayout_8.addWidget(self.groupBox_4)

        self.groupBox_5 = QGroupBox(self.Editor)
        self.groupBox_5.setObjectName(u"groupBox_5")
        self.verticalLayout_5 = QVBoxLayout(self.groupBox_5)
        self.verticalLayout_5.setObjectName(u"verticalLayout_5")
        self.formLayout = QFormLayout()
        self.formLayout.setObjectName(u"formLayout")
        self.label_2 = QLabel(self.groupBox_5)
        self.label_2.setObjectName(u"label_2")

        self.formLayout.setWidget(0, QFormLayout.ItemRole.LabelRole, self.label_2)

        self.focusModeCombo = QComboBox(self.groupBox_5)
        self.focusModeCombo.setObjectName(u"focusModeCombo")

        self.formLayout.setWidget(0, QFormLayout.ItemRole.FieldRole, self.focusModeCombo)

        self.label_3 = QLabel(self.groupBox_5)
        self.label_3.setObjectName(u"label_3")

        self.formLayout.setWidget(1, QFormLayout.ItemRole.LabelRole, self.label_3)

        self.editorWidthCombo = QComboBox(self.groupBox_5)
        self.editorWidthCombo.setObjectName(u"editorWidthCombo")

        self.formLayout.setWidget(1, QFormLayout.ItemRole.FieldRole, self.editorWidthCombo)

        self.label_4 = QLabel(self.groupBox_5)
        self.label_4.setObjectName(u"label_4")

        self.formLayout.setWidget(2, QFormLayout.ItemRole.LabelRole, self.label_4)

        self.blockquoteStyleCombo = QComboBox(self.groupBox_5)
        self.blockquoteStyleCombo.setObjectName(u"blockquoteStyleCombo")

        self.formLayout.setWidget(2, QFormLayout.ItemRole.FieldRole, self.blockquoteStyleCombo)

        self.label_5 = QLabel(self.groupBox_5)
        self.label_5.setObjectName(u"label_5")

        self.formLayout.setWidget(3, QFormLayout.ItemRole.LabelRole, self.label_5)

        self.underlineCombo = QComboBox(self.groupBox_5)
        self.underlineCombo.setObjectName(u"underlineCombo")

        self.formLayout.setWidget(3, QFormLayout.ItemRole.FieldRole, self.underlineCombo)


        self.verticalLayout_5.addLayout(self.formLayout)

        self.largeHeadingsCheckBox = QCheckBox(self.groupBox_5)
        self.largeHeadingsCheckBox.setObjectName(u"largeHeadingsCheckBox")

        self.verticalLayout_5.addWidget(self.largeHeadingsCheckBox)

        self.verticalSpacer_5 = QSpacerItem(20, 40, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.verticalLayout_5.addItem(self.verticalSpacer_5)


        self.verticalLayout_8.addWidget(self.groupBox_5)

        self.groupBox_6 = QGroupBox(self.Editor)
        self.groupBox_6.setObjectName(u"groupBox_6")
        self.verticalLayout_4 = QVBoxLayout(self.groupBox_6)
        self.verticalLayout_4.setObjectName(u"verticalLayout_4")
        self.cycleBulletPointsCheckBox = QCheckBox(self.groupBox_6)
        self.cycleBulletPointsCheckBox.setObjectName(u"cycleBulletPointsCheckBox")

        self.verticalLayout_4.addWidget(self.cycleBulletPointsCheckBox)

        self.autoMatchCheckBox = QCheckBox(self.groupBox_6)
        self.autoMatchCheckBox.setObjectName(u"autoMatchCheckBox")

        self.verticalLayout_4.addWidget(self.autoMatchCheckBox)

        self.matchedCharsButton = QPushButton(self.groupBox_6)
        self.matchedCharsButton.setObjectName(u"matchedCharsButton")

        self.verticalLayout_4.addWidget(self.matchedCharsButton)

        self.verticalSpacer_4 = QSpacerItem(20, 40, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.verticalLayout_4.addItem(self.verticalSpacer_4)


        self.verticalLayout_8.addWidget(self.groupBox_6)

        self.tabWidget.addTab(self.Editor, "")

        self.verticalLayout_7.addWidget(self.tabWidget)

        self.buttonBox = QDialogButtonBox(PreferencesDialog)
        self.buttonBox.setObjectName(u"buttonBox")
        self.buttonBox.setOrientation(Qt.Horizontal)
        self.buttonBox.setStandardButtons(QDialogButtonBox.Close)

        self.verticalLayout_7.addWidget(self.buttonBox)

#if QT_CONFIG(shortcut)
        self.label.setBuddy(self.cornersComboBox)
        self.label_6.setBuddy(self.tabWidthInput)
        self.label_2.setBuddy(self.focusModeCombo)
        self.label_3.setBuddy(self.editorWidthCombo)
        self.label_4.setBuddy(self.blockquoteStyleCombo)
        self.label_5.setBuddy(self.underlineCombo)
#endif // QT_CONFIG(shortcut)

        self.retranslateUi(PreferencesDialog)
        self.buttonBox.accepted.connect(PreferencesDialog.accept)
        self.buttonBox.rejected.connect(PreferencesDialog.reject)

        self.tabWidget.setCurrentIndex(0)


        QMetaObject.connectSlotsByName(PreferencesDialog)
    # setupUi

    def retranslateUi(self, PreferencesDialog):
        PreferencesDialog.setWindowTitle(QCoreApplication.translate("PreferencesDialog", u"Preferences", None))
        self.groupBox.setTitle(QCoreApplication.translate("PreferencesDialog", u"Display", None))
        self.clockCheckBox.setText(QCoreApplication.translate("PreferencesDialog", u"&Show current time in full screen mode", None))
        self.menuBarCheckBox.setText(QCoreApplication.translate("PreferencesDialog", u"&Hide menu bar in full screen mode", None))
        self.unbreakableSpaceCheckBox.setText(QCoreApplication.translate("PreferencesDialog", u"Show &Unbreakable Space", None))
        self.label.setText(QCoreApplication.translate("PreferencesDialog", u"Interface style", None))
        self.groupBox_2.setTitle(QCoreApplication.translate("PreferencesDialog", u"File Saving", None))
        self.autoSaveCheckBox.setText(QCoreApplication.translate("PreferencesDialog", u"&Auto save", None))
        self.backupCheckBox.setText(QCoreApplication.translate("PreferencesDialog", u"&Backup file on save", None))
        self.openDraftDirButton.setText(QCoreApplication.translate("PreferencesDialog", u"&View untitled drafts\u2026", None))
        self.currentBackupFolderText.setText(QCoreApplication.translate("PreferencesDialog", u"&", None))
        self.selectBackupFolderButton.setText(QCoreApplication.translate("PreferencesDialog", u"&Select Backup Folder Location\u2026", None))
        self.groupBox_3.setTitle(QCoreApplication.translate("PreferencesDialog", u"Session", None))
        self.rememberHistoryCheckBox.setText(QCoreApplication.translate("PreferencesDialog", u"&Remember recent files", None))
        self.restoreSessionCheckBox.setText(QCoreApplication.translate("PreferencesDialog", u"&Open last file on startup", None))
        self.groupBox_7.setTitle(QCoreApplication.translate("PreferencesDialog", u"Sidebar", None))
        self.folderViewShowAllFilesCheckBox.setText(QCoreApplication.translate("PreferencesDialog", u"&Show all files in folder view", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.General), QCoreApplication.translate("PreferencesDialog", u"General", None))
        self.groupBox_4.setTitle(QCoreApplication.translate("PreferencesDialog", u"Tabulation", None))
        self.spacesCheckBox.setText(QCoreApplication.translate("PreferencesDialog", u"&Insert spaces for tabs", None))
        self.label_6.setText(QCoreApplication.translate("PreferencesDialog", u"Tabulation width", None))
        self.groupBox_5.setTitle(QCoreApplication.translate("PreferencesDialog", u"Styling", None))
        self.label_2.setText(QCoreApplication.translate("PreferencesDialog", u"Focus mode", None))
        self.label_3.setText(QCoreApplication.translate("PreferencesDialog", u"Editor width", None))
        self.label_4.setText(QCoreApplication.translate("PreferencesDialog", u"Blockquote style", None))
        self.label_5.setText(QCoreApplication.translate("PreferencesDialog", u"Emphasis style", None))
        self.largeHeadingsCheckBox.setText(QCoreApplication.translate("PreferencesDialog", u"&Use large headings", None))
        self.groupBox_6.setTitle(QCoreApplication.translate("PreferencesDialog", u"Typing", None))
        self.cycleBulletPointsCheckBox.setText(QCoreApplication.translate("PreferencesDialog", u"C&ycle bullet point markers", None))
        self.autoMatchCheckBox.setText(QCoreApplication.translate("PreferencesDialog", u"&Automatically match characters", None))
        self.matchedCharsButton.setText(QCoreApplication.translate("PreferencesDialog", u"Customize &matched characters\u2026", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.Editor), QCoreApplication.translate("PreferencesDialog", u"Editor", None))
    # retranslateUi

