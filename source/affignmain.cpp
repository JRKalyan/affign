/// Name: affignmain.cpp
/// Purpose: Affign GUI Implementation
/// Author: John Kalyan
/// Copyright: (c) 2017 John Kalyan
/// License: MIT License

#include "affignmain.h"
#include "affignlogo.xpm"

// ****************************************************************************
// AffignApp
// ****************************************************************************

/// Event Handler
bool AffignApp::OnInit()
{
	if (!wxApp::OnInit()) return false;

	AffignMainFrame *frame = new AffignMainFrame("Affign");
	wxIcon icon(affignlogo_xpm);
	frame->SetIcon(icon);
	frame->Show(true);

	return true;
}


// ****************************************************************************
// AffignMainFrame
// ****************************************************************************

// Constructor ----------------------------------------------------------------

AffignMainFrame::AffignMainFrame(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title, wxPoint(-1, -1), wxSize(480, 320))
{

	alignerThread = NULL;

    // Menubar
    wxMenu *fileMenu = new wxMenu;
	fileMenu->Append(wxID_EXIT, "E&xit\tAlt-X", "Quit this program");

    wxMenu *helpMenu = new wxMenu;
	helpMenu->Append(wxID_ABOUT, "&About\tAlt-A", "Show about dialog");
	helpMenu->Append(ID_LICENSE, "&Licensing\tAlt-L", "Licensing information");
	
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(helpMenu, "&Help");

    SetMenuBar(menuBar);
	
	// Sizers

	wxPanel* panel = new wxPanel(this, -1);

	wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

	wxFlexGridSizer* directoryGrid = new wxFlexGridSizer(2, 3, 9, 10);

	wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);

	// flex sizer items (directories)

	wxStaticText* inputDirLabel = new wxStaticText(panel, -1, "Input Directory:");
	wxStaticText* outputDirLabel = new wxStaticText(panel, -1, "Output Directory:");

	inputDirDisplay = new wxTextCtrl(panel, -1, wxEmptyString, wxPoint(-1, -1),
		wxSize(-1, -1), wxTE_READONLY);
	outputDirDisplay = new wxTextCtrl(panel, -1, wxEmptyString, wxPoint(-1, -1),
		wxSize(-1, -1), wxTE_READONLY);
	
	wxButton* inputDirBtn = new wxButton(panel, ID_INPUT_SELECT, "Browse");
	wxButton* outputDirBtn = new wxButton(panel, ID_OUTPUT_SELECT, "Browse");
	
	// process and configure items

	wxButton* processBtn = new wxButton(panel, ID_PROCESS, "Process");
	wxButton* configureBtn = new wxButton(panel, ID_CONFIGURE, "Configure");
	wxButton* stopBtn = new wxButton(panel, ID_STOP, "Stop");

	// Progress Text

	progressText = new wxStaticText(panel, wxID_ANY, progressNA);

	// text display

	logDisplay = new wxTextCtrl(panel, -1, wxEmptyString, wxPoint(-1, -1),
		wxSize(-1, -1), wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH);


	directoryGrid->Add(inputDirLabel);
	directoryGrid->Add(inputDirDisplay, 1, wxEXPAND);
	directoryGrid->Add(inputDirBtn);
	directoryGrid->Add(outputDirLabel);
	directoryGrid->Add(outputDirDisplay, 1, wxEXPAND);
	directoryGrid->Add(outputDirBtn);
	directoryGrid->AddGrowableCol(1, 1);

	hbox->Add(configureBtn, 0, wxRIGHT, 10);
	hbox->Add(processBtn, 0, wxRIGHT, 10);
	hbox->Add(stopBtn);

	vbox->Add(directoryGrid, 0, wxALL | wxEXPAND, 10);
	vbox->Add(hbox, 0, wxALL | wxALIGN_CENTER, 10);
	vbox->Add(progressText, 0, wxALIGN_LEFT | wxLEFT, 10);
	vbox->Add(logDisplay, 1, wxTOP | wxEXPAND, 5);

	panel->SetSizer(vbox);

	SetIcon(wxICON(sample));
	SetMinSize(wxSize(480, 320));
	Centre();
}

// Event Handlers -------------------------------------------------------------

/// generates a close event from menu quit
void AffignMainFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

/// ensures that the thread is destroyed properly when frame closed
void AffignMainFrame::OnClose(wxCloseEvent& WXUNUSED(event))
{
	StopAlignerThread();
	while (1)
	{
		wxCriticalSectionLocker lock(alignerThreadCS);
		if (!alignerThread) break;
		wxThread::This()->Sleep(1);
	}
	delete imageAligner;
	Destroy();
}


/// displays informative message about affign
void AffignMainFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{

    CreateDialog(
		"Welcome to the Affign Photo Aligner.\n"
		"\n"
		"Affign uses face detection and landmarking "
		"to automatically align images based on face "
		"position for photo-a-day projects."
		"\n"
		"\n"
		"This software is licensed under the MIT license "
		"however it is linked to the following libraries "
		"which are licensed under their own terms. As "
		"per some of those conditions, their licenses "
		"are presented in the licensing section under \"Help\"."
		"\n"
		"\n"
		"OpenCV (3-Clause BSD-License)"
		"\n"
		"Dlib (Boost Software License)"
		"\n"
		"wxWidgets (wxWindows Library Licence)",
		"About Affign",
	     true
	);
}

/// shows licensing information
void AffignMainFrame::OnLicense(wxCommandEvent& WXUNUSED(event)) {
	wxString licenseText;
	wxString sep(
		"----------------------------------------"
		"----------------------------------------\n"
	);

	licenseText += "This software is licensed under it's own license, and "
		"it depends on other libraries with their own licenses.\n\n";

	licenseText += sep;
	licenseText += "Affign's License (MIT License)\n";
	licenseText += sep + "\n";
	licenseText += licenseMIT;
	licenseText += "\n\n";

	licenseText += sep;
	licenseText += "OpenCV's License (3-Clause BSD License)\n";
	licenseText += sep + "\n";
	licenseText += licenseOPENCV;
	licenseText += "\n\n";
	
	licenseText += sep;
	licenseText += "Dlib's License (Boost Software License)\n";
	licenseText += sep + "\n";
	licenseText += licenseDLIB;

	CreateDialog(licenseText, "Licensing information",true);
}

/// sets the directory with path separator when browse is pressed
void AffignMainFrame::OnDirSelect(wxCommandEvent& event) 
{
	if (alignerThread)
	{
		CreateDialog(
			"A process is currently running. Stop it if you\n"
			"wish to change this directory",
			error);
		return;
	}


	wxDirDialog dirDlg = new wxDirDialog(this);
	if (dirDlg.ShowModal() != wxID_OK)
	{
		return;
	}

	wxFileName pathFileName = wxFileName::DirName(dirDlg.GetPath());
	wxString path = pathFileName.GetPathWithSep();
	wxString dirName = wxEmptyString;

	if (event.GetId() == ID_INPUT_SELECT) 
	{
		mainConfig.inputDir = path;
		inputDirDisplay->SetValue(path);
		dirName = "input";
	}

	if (event.GetId() == ID_OUTPUT_SELECT) 
	{
		mainConfig.outputDir = path;
		outputDirDisplay->SetValue(path);
		dirName = "output";
	}

	LogText(dirName + " directory changed to: " + path, response);
}

/// creates or shows the configuration window
void AffignMainFrame::OnConfigure(wxCommandEvent& WXUNUSED(event))
{
	if (configFrame)
	{
		configFrame->Show();
		return;
	}

	configFrame = new AffignConfigFrame(this, "Configuration");
	configFrame->Show();
}

/// sets the main configuration and closes the configuration window
void AffignMainFrame::OnConfigureOk(wxCommandEvent& WXUNUSED(event)) {
	SetConfig(configFrame->tempConfig);
	configFrame->Close();
}

/// creates and starts a new thread if valid
void AffignMainFrame::OnProcess(wxCommandEvent& WXUNUSED(event)) 
{
	wxCriticalSectionLocker lock(alignerThreadCS);

	try
	{
		InputCheck(mainConfig.inputDir, mainConfig.outputDir);
	}
	catch (std::exception& e)
	{
		wxString msg = e.what();
		CreateDialog("Process cannot continue: " + msg, error);
		return;
	}

	alignerThread = new AffignThread(this, mainConfig);
	if (alignerThread->Run() != wxTHREAD_NO_ERROR)
	{
		CreateDialog("Process thread couldn't be created", error);
		delete alignerThread;
		alignerThread = NULL;
	}
}

/// tells thread to terminate when it can
void AffignMainFrame::OnStop(wxCommandEvent& WXUNUSED(event))
{
	StopAlignerThread();
}

/// logs the thread's message to the display
void AffignMainFrame::OnThreadMsg(wxThreadEvent& event)
{
	LogText(event.GetString(), (logStyle)event.GetInt());
}

/// creates a dialog with given message and style from thread
void AffignMainFrame::OnThreadDlg(wxThreadEvent& event)
{
	CreateDialog(event.GetString(), (logStyle)event.GetInt());
}

/// logs completion to display
void AffignMainFrame::OnThreadCompletion(wxThreadEvent& WXUNUSED(event))
{
	LogText("completed", success);
}

/// updates the progress text with new string
void AffignMainFrame::OnThreadProgress(wxThreadEvent& event)
{
	progressText->SetLabel(event.GetString());
}

// Methods --------------------------------------------------------------------

/// sets the main config to match the given config
void AffignMainFrame::SetConfig(AffignConfig& conf)
{
	mainConfig.video = conf.video;
	mainConfig.imgExtension = conf.imgExtension;
	mainConfig.frameRate = conf.frameRate;
	mainConfig.vidRoot = conf.vidRoot;
}

/// logs a message to the display with a given style
void AffignMainFrame::LogText(const wxString& text, logStyle style) 
{
	wxTextAttr attributes = wxTextAttr();
	wxColour colour;

	switch (style)
	{
	case message :
		colour = AFFIGN_BLACK;
		break;
	case response :
		colour = AFFIGN_BLUE;
		break;
	case warning :
		colour = AFFIGN_ORANGE;
		break;
	case error :
		colour = AFFIGN_RED;
		break;
	case success :
		colour = AFFIGN_GREEN;
		break;
	}

	attributes.SetTextColour(colour);
	logDisplay->SetDefaultStyle(attributes);

	*logDisplay << ">> " << text << "\n";
}

/// Creates a dialog with a given style
void AffignMainFrame::CreateDialog(const wxString& text, logStyle style)
{
	wxString dlgTitle;
	switch (style)
	{
	case message:
		dlgTitle = "Message";
		break;
	case response:
		dlgTitle = "Response";
		break;
	case warning:
		dlgTitle = "Warning";
		break;
	case error:
		wxBell();
		dlgTitle = "Error";
		break;
	case success:
		dlgTitle = "Success";
		break;
	}

	AffignDlg* dialog = new AffignDlg(this, dlgTitle, text);
	dialog->Show();
}

/// Creates a dialog with a given title / and with optional large option
void AffignMainFrame::CreateDialog(const wxString& text, 
	const wxString& title,
	bool large) {
	AffignDlg* dialog = new AffignDlg(this, title, text, large);
	dialog->Show();
}

/// deletes the aligner thread if possible
void AffignMainFrame::StopAlignerThread()
{
	wxCriticalSectionLocker lock(alignerThreadCS);
	
	if (alignerThread)
	{
		LogText("Alignment Stop Initiated", response);
		if (alignerThread->Delete() != wxTHREAD_NO_ERROR)
		{
			LogText("Process thread could not be deleted", error);
		}
	}
}

/// throws if input/output directory is not valid or process is ongoing already
void AffignMainFrame::InputCheck(
	const wxString& input, 
	const wxString& output)
{
	// make sure this is called within a critical section for accuracy

	if (alignerThread)
	{
		throw AffignException("There is already an ongoing process");
	}

	if (!wxFileName::IsDirReadable(input))
	{
		throw AffignException("Input Directory is not readable");
	}

	if (!wxFileName::IsDirWritable(output))
	{
		throw AffignException("Output Directory is not writable");
	}
}


// ****************************************************************************
// AffignConfigFrame
// ****************************************************************************

// Constructor ----------------------------------------------------------------

AffignConfigFrame::AffignConfigFrame(
	wxWindow* parent, 
	const wxString& title)
	: wxFrame(parent, -1, title, wxPoint(-1, -1), AFFIGN_CONFIG_SIZE,
		wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX))
{
	processFps = false;

	wxPanel* panel = new wxPanel(this);

	// sizers
	wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

	wxStaticBoxSizer* radioBox = new wxStaticBoxSizer(
		wxHORIZONTAL,
		panel,
		"Output Type");

	wxFlexGridSizer* fbox = new wxFlexGridSizer(2, 2, 10, 10);
	wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);


	// two radio buttons

	wxRadioButton* imgRB = new wxRadioButton(
		radioBox->GetStaticBox(),
		ID_CONFIG_IMG,
		"Output as a sequence of images");
	wxRadioButton* vidRB = new wxRadioButton(
		radioBox->GetStaticBox(),
		ID_CONFIG_VID,
		"Output as a video");

	// file extension choice
	wxArrayString choices;
	choices.Add(".bmp");
	choices.Add(".jpg");
	choices.Add(".png");
	choices.Add(".tiff");

	wxStaticText* extLabel = new wxStaticText(panel, -1, "Image Sequence File Extension");
	wxChoice* extChoice = new wxChoice(panel, ID_CONFIG_EXT, wxPoint(-1, -1),
		wxSize(-1, -1), choices);

	extChoice->SetSelection(bmp);

	// framerate selection
	wxStaticText* fpsLabel = new wxStaticText(panel, -1, "Video Frame Rate");
	fpsCtrl = new wxTextCtrl(panel, ID_CONFIG_FPS,
		wxString::Format("%d", AFFIGN_DEFAULT_FRAMERATE), wxDefaultPosition, wxDefaultSize,
		0, wxTextValidator(wxFILTER_DIGITS));

	// root name selection
	wxStaticText* rootLabel = new wxStaticText(panel, -1, "Video Name");
	rootCtrl = new wxTextCtrl(panel, ID_CONFIG_ROOT,
		"default", wxDefaultPosition, wxDefaultSize,
		0, wxTextValidator(wxFILTER_ALPHANUMERIC));

	// ok/cancel buttons
	wxButton* okBtn = new wxButton(panel, ID_CONFIG_OK, "OK");
	wxButton* cancelBtn = new wxButton(panel, ID_CONFIG_CANCEL, "Cancel");

	radioBox->Add(imgRB, 0, wxALL, 10);
	radioBox->Add(vidRB, 0, wxALL, 10);

	fbox->Add(extLabel);
	fbox->Add(extChoice);
	fbox->Add(rootLabel);
	fbox->Add(rootCtrl);
	fbox->Add(fpsLabel);
	fbox->Add(fpsCtrl);

	hbox->Add(okBtn, 0, wxALIGN_BOTTOM, 0);
	hbox->Add(cancelBtn, 0, wxALIGN_BOTTOM, 0);

	vbox->Add(radioBox, 0, wxEXPAND | wxALL, 10);
	vbox->Add(fbox, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);
	vbox->Add(hbox, 1, wxALIGN_RIGHT | wxALL, 10);

	panel->SetSizer(vbox);
	SetMinSize(AFFIGN_CONFIG_SIZE);
}

// Event Handlers -------------------------------------------------------------

/// close event without saving config
void AffignConfigFrame::OnCancel(wxCommandEvent& WXUNUSED(event))
{
	Close();
}

/// hides the window
void AffignConfigFrame::OnClose(wxCloseEvent& WXUNUSED(event))
{
	Show(false);
}

/// sets config to output as image sequence
void AffignConfigFrame::OnImgRad(wxCommandEvent& WXUNUSED(event))
{
	tempConfig.video = false;
}

/// sets config to output as video
void AffignConfigFrame::OnVidRad(wxCommandEvent& WXUNUSED(event))
{
	tempConfig.video = true;
}

/// tells the idle handler to process root input
void AffignConfigFrame::OnRoot(wxCommandEvent& WXUNUSED(event))
{
	processRoot = true;
}

/// sets the config image extension to the choice
void AffignConfigFrame::OnExt(wxCommandEvent& event)
{
	int choice = event.GetSelection();
	if (choice == wxNOT_FOUND)
	{
		choice = bmp;
	}

	tempConfig.imgExtension = (extType)choice;
}

/// tells the idle handler to process the framerate
void AffignConfigFrame::OnFps(wxCommandEvent& WXUNUSED(event))
{
	processFps = true;
}

/// processes the framerate and root name text controls if updated
void AffignConfigFrame::OnIdle(wxIdleEvent& WXUNUSED(event))
{
	if (processFps)
	{
		wxString fpsString = fpsCtrl->GetValue();
		int tempFps = strtol(fpsString.c_str(), NULL, 10);

		int maxFps = 200;
		int minFps = 1;
		if (tempFps < minFps)
		{
			tempFps = minFps;
		}
		if (tempFps > maxFps)
		{
			tempFps = maxFps;
		}

		tempConfig.frameRate = tempFps;
		if (fpsCtrl->GetValue() != wxEmptyString)
		{
			fpsCtrl->SetValue(wxString::Format("%d", tempFps));
			fpsCtrl->SetInsertionPointEnd();
		}
	}

	if (processRoot)
	{
		tempConfig.vidRoot = rootCtrl->GetValue();
	}

	processRoot = false;
	processFps = false;
}


// ****************************************************************************
// AffignThread
// ****************************************************************************

// Constructor ----------------------------------------------------------------

AffignThread::AffignThread(AffignMainFrame* framePtr, AffignConfig conf)
	: wxThread(wxTHREAD_DETACHED), handler(framePtr), threadConfig(conf)
{
}

// Methods --------------------------------------------------------------------

/// aligns and outputs images
wxThread::ExitCode AffignThread::Entry()
{
	wxArrayString files;
	wxDir::GetAllFiles(threadConfig.inputDir, &files, wxEmptyString, wxDIR_FILES);

	int numSuccess = 0;
	wxArrayString failed;

	if (!handler->imageAligner)
	{
		SendMsg("Initializing Aligner");

		try
		{
			handler->imageAligner = new AffignAligner();
			SendMsg("Initialization Complete", success);
		}
		catch (std::exception const& e)
		{
			SendMsg(e.what(), error);
			UpdateProgress(progressComplete);
			wxQueueEvent(handler, new wxThreadEvent(wxEVT_THREAD, ID_THREAD_COMPLETE));
			return (wxThread::ExitCode)(-1);
		}
	}

	if (TestDestroy()) {
		UpdateProgress(progressComplete);
		wxQueueEvent(handler, new wxThreadEvent(wxEVT_THREAD, ID_THREAD_COMPLETE));
		return (wxThread::ExitCode)(0);
	}

	try
	{
		ReferenceCheck(files);
	}
	catch (std::exception const& e)
	{
		SendMsg(e.what(), error);
		UpdateProgress(progressComplete);
		wxQueueEvent(handler, new wxThreadEvent(wxEVT_THREAD, ID_THREAD_COMPLETE));
		return (wxThread::ExitCode)(-1);
	}
	SendMsg("Reference successfully set", success);

	wxString vidRoot = threadConfig.vidRoot;
	wxString videoOutputName = threadConfig.outputDir + vidRoot;
	wxString nameExt;
	int numFiles = files.GetCount();

	if (threadConfig.video) {
		nameExt = ".avi";
		videoOutputName += nameExt;
		handler->imageAligner->AddVideo(videoOutputName.c_str(), threadConfig.frameRate);
	}
	else {
		switch (threadConfig.imgExtension)
		{
		case bmp:
			nameExt = ".bmp";
			break;
		case jpg:
			nameExt = ".jpg";
			break;
		case png:
			nameExt = ".png";
			break;
		case tiff:
			nameExt = ".tiff";
			break;
		}
	}

	for (int i = 0; i < numFiles && !TestDestroy(); ++i) {

		const char *imagePath = files[i].c_str();

		const char* imageOutputName;

		wxString imageOutputTemp;
		if (!threadConfig.video)
		{
			wxFileName inputName(imagePath);
			imageOutputTemp = threadConfig.outputDir + inputName.GetName() + nameExt;
			imageOutputName = imageOutputTemp.c_str();
		}
		else
		{
			imageOutputName = NULL;
		}

		try
		{
			SendMsg("attempting to align: " + files[i]);
			handler->imageAligner->Align(imagePath, imageOutputName);
			numSuccess++;
		}
		catch (std::exception const& e)
		{
			wxString whatMsg = e.what();
			SendMsg("alignment failed: " + whatMsg, warning);
			failed.Add(imagePath);
		}

		float percent = ((float)(i + 1) / (float)numFiles) * 100;
		wxString progString = wxString::Format("Progress: %d/%d (%d",
			i + 1, numFiles, (int)percent) + "%)";
		UpdateProgress(progString);
	}

	if (threadConfig.video)
	{
		handler->imageAligner->RemoveVideo();
	}

	wxTextFile logFile(threadConfig.outputDir + "log.txt");

	if (logFile.Exists())
	{
		logFile.Open();
		logFile.Clear();
	}
	else
	{
		logFile.Create();
		logFile.Open();
	}

	int numFailed = failed.GetCount();
	int i = 0;

	while (logFile.IsOpened())
	{

		if (i == 0)
		{
			logFile.AddLine(wxString::Format("Successfully aligned: %d image(s)",
				numSuccess));
			logFile.AddLine(wxString::Format("Failed to align: %d files(s)", numFailed));
			logFile.AddLine(wxEmptyString);

			if (numFailed)
			{
				logFile.AddLine("Failed file(s):");
			}
		}

		if (i >= numFailed)
		{
			break;
		}

		logFile.AddLine(failed[i]);
		i++;
	}

	logFile.Write();
	logFile.Close();

	UpdateProgress(progressComplete);
	wxQueueEvent(handler, new wxThreadEvent(wxEVT_THREAD, ID_THREAD_COMPLETE));
	return (wxThread::ExitCode)0;
}

/// sets the reference as the first valid image if possible
void AffignThread::ReferenceCheck(const wxArrayString& files)
{
	size_t count = files.GetCount();

	for (int i = 0; i < count && !TestDestroy(); ++i)
	{
		try
		{
			SendMsg("Trying to set " + files[i] + " as the reference");
			handler->imageAligner->SetReference(files[i].c_str());
			return;
		}
		catch (std::exception& e)
		{
			wxString whatMsg = e.what();
			SendMsg("Reference could not be set: " + whatMsg, warning);
		}
	}

	throw AffignException("No valid reference set");
}

/// sends a message event to the gui thread
void AffignThread::SendMsg(const wxString msg, logStyle style)
{
	wxThreadEvent* evt = new wxThreadEvent(wxEVT_THREAD, ID_THREAD_MSG);
	evt->SetString(msg);
	evt->SetInt(style);
	wxQueueEvent(handler, evt);
}

/// sends a dialog event to the gui thread
void AffignThread::SendDlg(const wxString msg, logStyle style)
{
	wxThreadEvent* evt = new wxThreadEvent(wxEVT_THREAD, ID_THREAD_DLG);
	evt->SetString(msg);
	evt->SetInt(style);
	wxQueueEvent(handler, evt);
}

/// sends a progress event to the gui thread
void AffignThread::UpdateProgress(const wxString msg)
{
	wxThreadEvent* evt = new wxThreadEvent(wxEVT_THREAD, ID_THREAD_PROG);
	evt->SetString(msg);
	wxQueueEvent(handler, evt);
}

// Destructor -----------------------------------------------------------------

AffignThread::~AffignThread()
{
	wxCriticalSectionLocker lock(handler->alignerThreadCS);
	handler->alignerThread = NULL;
}


// ****************************************************************************
// AffignDlg
// ****************************************************************************

// Constructor ----------------------------------------------------------------

AffignDlg::AffignDlg(
	wxWindow* parent,
	const wxString& title,
	const wxString& text,
	bool large)
	: wxFrame(parent, -1, title, wxPoint(-1, -1), AFFIGN_DLG_SIZE)
{
	panel = new wxPanel(this);
	vbox = new wxBoxSizer(wxVERTICAL);
	hbox1 = new wxBoxSizer(wxHORIZONTAL);
	hbox2 = new wxBoxSizer(wxHORIZONTAL);

	if (!large) {
		wxStaticText* msg = new wxStaticText(panel, -1, text);
		hbox1->Add(msg, 1, wxALIGN_TOP | wxALL | wxEXPAND, 10);
	}
	else {
		wxTextCtrl* ctrlMsg = new wxTextCtrl(panel, -1, text, wxDefaultPosition,
			wxDefaultSize, wxTE_READONLY | wxTE_MULTILINE);
		hbox1->Add(ctrlMsg, 1, wxALL | wxEXPAND, 10);
	}
	
	okBtn = new wxButton(panel, wxID_EXIT, "OK");
	hbox2->Add(okBtn, 0, wxRIGHT | wxBOTTOM | wxLEFT, 10);
	
	vbox->Add(hbox1, 1, wxALIGN_CENTER | wxEXPAND, 0);
	vbox->Add(hbox2, 0, wxALIGN_RIGHT, 0);

	panel->SetSizer(vbox);
	if (large) {
		SetSize(AFFIGN_DLG_SIZE_LARGE);
		SetMinSize(AFFIGN_DLG_SIZE_LARGE);
	}
	else {
		SetMinSize(AFFIGN_DLG_SIZE);
	}

	Center();
}

// Event Handlers -------------------------------------------------------------

/// generates a close event
void AffignDlg::OnOK(wxCommandEvent& WXUNUSED(event))
{
	Close(true);
}


// ****************************************************************************
// AffignConfig
// ****************************************************************************

// Constructor ----------------------------------------------------------------

AffignConfig::AffignConfig()
{
	frameRate = AFFIGN_DEFAULT_FRAMERATE;
	video = false;
	imgExtension = bmp;
	vidRoot = "default";
	inputDir = wxEmptyString;
	outputDir = wxEmptyString;
}


// ****************************************************************************
// Implement
// ****************************************************************************

wxIMPLEMENT_APP(AffignApp);