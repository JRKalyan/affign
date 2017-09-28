/// Name: affignmain.h
/// Purpose: Affign GUI / Header
/// Author: John Kalyan
/// Copyright: (c) 2017 John Kalyan
/// License: MIT License

// ****************************************************************************
// headers
// ****************************************************************************

#pragma once

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/dir.h"
#include "wx/filename.h"
#include "wx/textfile.h"

#ifndef wxHAS_IMAGES_IN_RESOURCES
#include "../sample.xpm"
#endif

#include "affignaligner.h"
#include "affignexcept.h"

// ****************************************************************************
// constants
// ****************************************************************************

/// Widget IDs
enum
{
	// main frame IDs
	ID_INPUT_SELECT = wxID_HIGHEST + 1,
	ID_OUTPUT_SELECT,
	ID_PROCESS,
	ID_CONFIGURE,
	ID_STOP,

	// configuration frame IDs
	ID_CONFIG_OK,
	ID_CONFIG_CANCEL,
	ID_CONFIG_VID,
	ID_CONFIG_IMG,
	ID_CONFIG_EXT,
	ID_CONFIG_FPS,
	ID_CONFIG_ROOT,

	// thread event IDs
	ID_THREAD_COMPLETE,
	ID_THREAD_MSG,
	ID_THREAD_DLG,
	ID_THREAD_PROG
};

/// Styles for messages given to log display
enum logStyle {
	message, // eg attempting to align
	response, // eg user clicks stop
	warning, // eg failed to align certain image
	error, // eg reference failed to be set, fatal to process
	success // eg  complete aligner initialization
};

/// Extensions for image sequence
enum extType {
	bmp = 0,
	jpg,
	png,
	tiff
};

namespace {
	const wxSize AFFIGN_MAIN_SIZE(480, 320);
	const wxSize AFFIGN_DLG_SIZE(300, 150);
	const wxSize AFFIGN_CONFIG_SIZE(400, 270);

	const wxColour AFFIGN_BLACK(0, 0, 0);
	const wxColour AFFIGN_RED(180, 0, 0);
	const wxColour AFFIGN_ORANGE(255, 140, 0);
	const wxColour AFFIGN_BLUE(0, 0, 180);
	const wxColour AFFIGN_GREEN(0, 180, 0);

	const int AFFIGN_DEFAULT_FRAMERATE = 30;

}

// ****************************************************************************
// Classes
// ****************************************************************************


class AffignThread;
class AffignConfigFrame;

struct AffignConfig
{
	AffignConfig();

	bool video;
	extType imgExtension;
	int frameRate;
	wxString vidRoot;
	wxString inputDir;
	wxString outputDir;
};

class AffignApp : public wxApp
{
public:

	virtual bool OnInit() wxOVERRIDE;
};

class AffignMainFrame : public wxFrame
{
public:

	AffignMainFrame(const wxString& title);
	~AffignMainFrame() {};
	
	// Event Handlers

	void OnQuit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnDirSelect(wxCommandEvent& event);
	void OnConfigure(wxCommandEvent& event);
	void OnConfigureOk(wxCommandEvent& event);
	void OnProcess(wxCommandEvent& event);
	void OnStop(wxCommandEvent& event);
	void OnClose(wxCloseEvent& event);

	// Thread Event Handlers

	void OnThreadMsg(wxThreadEvent& event);
	void OnThreadDlg(wxThreadEvent& event);
	void OnThreadCompletion(wxThreadEvent& event);
	void OnThreadProgress(wxThreadEvent& event);

protected:

	AffignConfig mainConfig;

	wxTextCtrl* inputDirDisplay;
	wxTextCtrl* outputDirDisplay;
	wxStaticText* progressText;
	wxTextCtrl* logDisplay;

	AffignThread* alignerThread;
	wxCriticalSection alignerThreadCS;
	friend class AffignThread;

	AffignConfigFrame* configFrame = (AffignConfigFrame*)NULL;

	// Only used by the alignerThread
	AffignAligner* imageAligner = NULL;
	
	// Methods

	void LogText(const wxString& text, logStyle style = message);
	void InputCheck(const wxString& input, const  wxString& output);
	void StopAlignerThread();
	void CreateDialog(const wxString& text, logStyle style);
	void SetConfig(AffignConfig& conf);

	wxDECLARE_EVENT_TABLE();
};

class AffignConfigFrame : public wxFrame
{
public:

	AffignConfigFrame(wxWindow* parent, const wxString& title);

	void OnExt(wxCommandEvent& event);
	void OnFps(wxCommandEvent& event);
	void OnImgRad(wxCommandEvent& event);
	void OnVidRad(wxCommandEvent& event);
	void OnCancel(wxCommandEvent& event);
	void OnClose(wxCloseEvent& event);
	void OnIdle(wxIdleEvent& event);
	void OnRoot(wxCommandEvent& event);

	AffignConfig tempConfig;

protected:

	wxTextCtrl* fpsCtrl;
	wxTextCtrl* rootCtrl;

	bool processRoot;
	bool processFps;

	DECLARE_EVENT_TABLE();
};

class AffignThread : public wxThread
{
public:
	
	AffignThread(AffignMainFrame* framePtr, AffignConfig conf); 
	~AffignThread();

protected:

	AffignConfig threadConfig;

	AffignMainFrame* handler;

	AffignThread() {};

	// Methods

	virtual wxThread::ExitCode Entry();
	void ReferenceCheck(const wxArrayString& files);
	void SendMsg(const wxString msg, logStyle style = message);
	void SendDlg(const wxString msg, logStyle style = error);
	void UpdateProgress(const wxString msg);
};

class AffignDlg : public wxFrame
{
public:

	AffignDlg(wxWindow* parent, const wxString& title, const wxString& text);

	void OnOK(wxCommandEvent& event);

protected:

	wxPanel* panel;
	wxBoxSizer* vbox;
	wxBoxSizer* hbox1;
	wxBoxSizer* hbox2;

	wxButton* okBtn;
	wxStaticText* msg;

	wxDECLARE_EVENT_TABLE();
};


// ****************************************************************************
// Event Tables
// ****************************************************************************

/// Main Frame Event Table
wxBEGIN_EVENT_TABLE(AffignMainFrame, wxFrame)
EVT_MENU(wxID_EXIT, AffignMainFrame::OnQuit)
EVT_MENU(wxID_ABOUT, AffignMainFrame::OnAbout)
EVT_BUTTON(ID_INPUT_SELECT, AffignMainFrame::OnDirSelect)
EVT_BUTTON(ID_OUTPUT_SELECT, AffignMainFrame::OnDirSelect)
EVT_BUTTON(ID_CONFIGURE, AffignMainFrame::OnConfigure)
EVT_BUTTON(ID_CONFIG_OK, AffignMainFrame::OnConfigureOk)
EVT_BUTTON(ID_PROCESS, AffignMainFrame::OnProcess)
EVT_BUTTON(ID_STOP, AffignMainFrame::OnStop)
EVT_CLOSE(AffignMainFrame::OnClose)
EVT_THREAD(ID_THREAD_COMPLETE, AffignMainFrame::OnThreadCompletion)
EVT_THREAD(ID_THREAD_DLG, AffignMainFrame::OnThreadDlg)
EVT_THREAD(ID_THREAD_MSG, AffignMainFrame::OnThreadMsg)
EVT_THREAD(ID_THREAD_PROG, AffignMainFrame::OnThreadProgress)
wxEND_EVENT_TABLE()

/// Configuration Event Table
wxBEGIN_EVENT_TABLE(AffignConfigFrame, wxFrame)
EVT_RADIOBUTTON(ID_CONFIG_IMG, AffignConfigFrame::OnImgRad)
EVT_RADIOBUTTON(ID_CONFIG_VID, AffignConfigFrame::OnVidRad)
EVT_CLOSE(AffignConfigFrame::OnClose)
EVT_TEXT(ID_CONFIG_FPS, AffignConfigFrame::OnFps)
EVT_TEXT(ID_CONFIG_ROOT, AffignConfigFrame::OnRoot)
EVT_CHOICE(ID_CONFIG_EXT, AffignConfigFrame::OnExt)
EVT_IDLE(AffignConfigFrame::OnIdle)
EVT_BUTTON(ID_CONFIG_CANCEL, AffignConfigFrame::OnCancel)
wxEND_EVENT_TABLE()

/// Dialog Event Table
wxBEGIN_EVENT_TABLE(AffignDlg, wxFrame)
EVT_BUTTON(wxID_EXIT, AffignDlg::OnOK)
wxEND_EVENT_TABLE()