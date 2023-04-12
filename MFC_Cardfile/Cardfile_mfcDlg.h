
// Cardfile_mfcDlg.h: файл заголовка
//



// Диалоговое окно CCardfilemfcDlg
class CCardfilemfcDlg : public CDialogEx
{
// Создание
public:
	CCardfilemfcDlg(CWnd* pParent = nullptr);	// стандартный конструктор
	
// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CARDFILE_MFC_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// поддержка DDX/DDV


// Реализация
protected:
	HICON m_hIcon;

	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	void CCardfilemfcDlg::LoadCardsFromFile();
	void CCardfilemfcDlg::SaveCardsToFile();
	void CCardfilemfcDlg::showInfoPanel();
	void CCardfilemfcDlg::hideInfoPanel();
	int counterId;
	afx_msg void OnEnChangeName();
	afx_msg void OnLbnSelchangeListbox();
	CString paramName;
	CString paramSurname;

	CButton m_radioMale;
	CButton m_radioFemale;
	CButton buttonDelete;
	CButton m_ButtonOk;
	afx_msg void OnBnClickedButtondelete();
	afx_msg void OnBnClickedButtonadd();
	afx_msg void OnBnClickedButtonsave();

	int currentSelection;
	void CCardfilemfcDlg::SaveCurrentSelection();
	CComboBox m_comboBox;
	int paramChilds;
	CDateTimeCtrl m_dateTimePicker;
	COleDateTime m_dateTime;
	CListBox m_listBox;

	void AddCard(int id, CString& name, CString& surname, int numberButtonGender, COleDateTime& dateTimeValue, int numberFamilyStatus, int countChilds, bool checkAutomobile);

	struct CardInfo
	{
		int id;
		CString name;
		CString surname;
		int numberButtonGender; //значения кнопки, сохраняет id кнопки
		COleDateTime dateTimeValue; // значения даты из элемента DateTimePicker
		int numberFamilyStatus; // Значение семейного положения 0-3. 0 - не женат, 1 - женат, 2 - в разводе, 3 - вдовец
		int countChilds; // Количество детей
		bool checkAutomobile; // наличие автомобиля, true - есть, false - нет
	public:
		//оператор сравнения всех полей
		bool operator==(const CardInfo& other) const {
			return id == other.id &&
				name == other.name &&
				surname == other.surname &&
				numberButtonGender == other.numberButtonGender &&
				dateTimeValue == other.dateTimeValue &&
				numberFamilyStatus == other.numberFamilyStatus &&
				countChilds == other.countChilds &&
				checkAutomobile == other.checkAutomobile;
				
	
		}
		
	};

	CArray<CardInfo, CardInfo&> m_cards;
	CArray<CardInfo, CardInfo&> m_cardsReserv; // копия списка для резерва
	afx_msg void OnEnChangeSurname();
	afx_msg void OnDtnDatetimeChangeBirthdate(NMHDR* pNMHDR, LRESULT* pResult);
	int numberStatus;
	BOOL checkCar;

	void UpdateButtonDeleteState();
	void CCardfilemfcDlg::SelectLastItemInListBox();
	bool CCardfilemfcDlg::AreCardsEqual(const CArray<CardInfo, CardInfo&>& array1, const CArray<CardInfo, CardInfo&>& array2);

	CEdit m_paramName;
	CEdit m_paramSurname;
	CStatic m_textname;
	CStatic m_textsurname;
	CStatic m_Gendertext;
	CStatic m_datetext;
	CStatic m_FamilyStatustext;
	CStatic m_childstext;
	CButton m_checkcar;
	CSpinButtonCtrl m_spinChild;
	CEdit m_Childs;
	CStatic m_infotext;
	afx_msg void OnBnClickedButtonok();
	afx_msg void OnEnSetfocuschildcount();
	afx_msg void OnBnClickedButtonexit();
};
