#define kMBarDisplayed 128
#define rUserAlert 129
#define	rAppStringsID			128
#define kOpenPrefKey			1
#define	kMaxDocumentCount		100		// maximum number of documents allowed

enum {
	sApplicationName = 1,
	sTranslationLockedErr,
	sTranslationErr,
	sOpeningErr,
	sReadErr,				// 5
	sWriteToBusyFileErr,
	sBusyOpen,
	sChooseFile,
	sChooseFolder,
	sChooseVolume,			// 10
	sCreateFolder,
	sChooseObject,
	sSaveCopyMessage,
	slSavePromptIndex,
	slClosingIndex,			// 15
	slQuittingIndex,
	sAddRemoveTitle,
	sLowMemoryErr
};

#define mApple 128
#define mAppleAbout 1

#define mFile 129
#define mFileOpen 1
#define mFileQuit 2

#define mEdit 130
#define mEditCopy  1
#define mEditSelectAll  2

#define mWindow 131


#define APP_NAME_STRING "\pQueryCSV"
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 320
#define BITMAP_SIZE 16
