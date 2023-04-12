
// Cardfile_mfcDlg.cpp: файл реализации

#include "pch.h"
#include "framework.h"
#include "Cardfile_mfc.h"
#include "Cardfile_mfcDlg.h"
#include "afxdialogex.h"

#include <iostream>
#include <atlfile.h>
#include <atlconv.h>

#include <fstream>

#include <locale>
#include <codecvt>
#include <string>
#include <sstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Диалоговое окно CCardfilemfcDlg


CCardfilemfcDlg::CCardfilemfcDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CARDFILE_MFC_DIALOG, pParent)
	, checkCar(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	currentSelection = -1;
}

void CCardfilemfcDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_NAME, paramName);
	DDX_Text(pDX, IDC_surname, paramSurname);
	DDX_Control(pDX, IDC_male, m_radioMale);
	DDX_Control(pDX, IDC_female, m_radioFemale);
	DDX_Control(pDX, IDC_FAMILYSTATUS, m_comboBox);
	DDX_Text(pDX, IDC_childcount, paramChilds);
	DDX_Control(pDX, IDC_BIRTHDATE, m_dateTimePicker);
	DDX_Control(pDX, IDC_LISTBOX, m_listBox);
	DDX_DateTimeCtrl(pDX, IDC_BIRTHDATE, m_dateTime);
	DDX_CBIndex(pDX, IDC_FAMILYSTATUS, numberStatus);
	DDX_Check(pDX, IDC_checkcar, checkCar);
	DDX_Control(pDX, IDC_BUTTONdelete, buttonDelete);
	DDX_Control(pDX, IDC_NAME, m_paramName);
	DDX_Control(pDX, IDC_surname, m_paramSurname);
	DDX_Control(pDX, IDC_textname, m_textname);
	DDX_Control(pDX, IDC_textsurname, m_textsurname);
	DDX_Control(pDX, IDC_textgender, m_Gendertext);
	DDX_Control(pDX, IDC_textdate, m_datetext);
	DDX_Control(pDX, IDC_textfamily, m_FamilyStatustext);
	DDX_Control(pDX, IDC_textchilds, m_childstext);
	DDX_Control(pDX, IDC_checkcar, m_checkcar);
	DDX_Control(pDX, IDC_SPINchild, m_spinChild);
	DDX_Control(pDX, IDC_childcount, m_Childs);
	DDX_Control(pDX, IDC_INFO, m_infotext);
	DDX_Control(pDX, IDC_BUTTONok, m_ButtonOk);
}


BEGIN_MESSAGE_MAP(CCardfilemfcDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_CHANGE(IDC_NAME, &CCardfilemfcDlg::OnEnChangeName)
	ON_BN_CLICKED(IDC_BUTTONdelete, &CCardfilemfcDlg::OnBnClickedButtondelete)
	ON_BN_CLICKED(IDC_BUTTONadd, &CCardfilemfcDlg::OnBnClickedButtonadd)
	ON_BN_CLICKED(IDC_BUTTONsave, &CCardfilemfcDlg::OnBnClickedButtonsave)
	ON_EN_CHANGE(IDC_surname, &CCardfilemfcDlg::OnEnChangeSurname)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_BIRTHDATE, &CCardfilemfcDlg::OnDtnDatetimeChangeBirthdate)
	ON_LBN_SELCHANGE(IDC_LISTBOX, &CCardfilemfcDlg::OnLbnSelchangeListbox)
	ON_BN_CLICKED(IDC_BUTTONok, &CCardfilemfcDlg::OnBnClickedButtonok)
	ON_EN_SETFOCUS(IDC_childcount, &CCardfilemfcDlg::OnEnSetfocuschildcount)
	ON_BN_CLICKED(IDC_BUTTONexit, &CCardfilemfcDlg::OnBnClickedButtonexit)
END_MESSAGE_MAP()


// Обработчики сообщений CCardfilemfcDlg

BOOL CCardfilemfcDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// Задает значок для этого диалогового окна.  Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок
	SetWindowText(_T("Картотека"));
	paramName = _T("Новая");
	paramSurname = _T("карточка");
	paramChilds = 0;
	UpdateData(FALSE);
	m_radioMale.SetCheck(BST_CHECKED);
	m_comboBox.SetCurSel(0);
	COleDateTime defaultDate(1970, 1, 1, 0, 0, 0);
	m_dateTimePicker.SetTime(defaultDate);
	//m_listBox.AddString((paramName + ' ' + paramSurname));
	//m_listBox.SubclassDlgItem(IDC_LISTBOX, this);
	CheckRadioButton(IDC_female, IDC_male, IDC_male);
	CButton* pCheckBox = (CButton*)GetDlgItem(IDC_checkcar); // Устанавливаем Check Box "есть автомобиль" в статус 0.
	pCheckBox->SetCheck(0);
	m_spinChild.SetBuddy(&m_Childs);
	m_spinChild.SetRange(0, 12);
	m_spinChild.SetPos(0);
	//UpdateData(TRUE);
	LoadCardsFromFile();
	counterId = m_listBox.GetCount();
	//Скроем правую панель с информацией
	hideInfoPanel();
	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
}

void CCardfilemfcDlg::LoadCardsFromFile()
{
	CAtlFile file;
	if (SUCCEEDED(file.Create(_T("cards.txt"), GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING)))
	{
		CAtlFileMapping<> mapping;
		if (SUCCEEDED(mapping.MapFile(file)))
		{
			const char* pData = (const char*)mapping.GetData();
			const char* pEnd = pData + mapping.GetMappingSize();
			while (pData < pEnd)
			{
				const char* pLineEnd = strchr(pData, '\n');
				if (!pLineEnd)
				{
					pLineEnd = pEnd;
				}
				CStringA line(pData, int(pLineEnd - pData));
				CString strLine = CA2T(line);

				CardInfo card;
				CStringArray tokens;
				int delimiterPos = 0, prevDelimiterPos = 0;

				while ((delimiterPos = strLine.Find(_T(','), prevDelimiterPos)) != -1)
				{
					CString token = strLine.Mid(prevDelimiterPos, delimiterPos - prevDelimiterPos);
					tokens.Add(token);
					prevDelimiterPos = delimiterPos + 1;
				}

				// Добавляем последний токен после последнего разделителя
				tokens.Add(strLine.Mid(prevDelimiterPos));

				// Присваиваем значения переменным
				card.id = _ttoi(tokens[0]);
				card.name = tokens[1];
				card.surname = tokens[2];
				card.numberButtonGender = _ttoi(tokens[3]);
				card.numberFamilyStatus = _ttoi(tokens[4]);
				card.dateTimeValue.ParseDateTime(tokens[5], VAR_DATEVALUEONLY);
				card.countChilds = _ttoi(tokens[6]);
				card.checkAutomobile = tokens[7];

				m_cards.Add(card);
				m_cardsReserv.Add(card);
				m_listBox.AddString(card.name + _T(" ") + card.surname);

				pData = pLineEnd + 1;
			}
		}
	}
	UpdateButtonDeleteState();
}

void CCardfilemfcDlg::SaveCardsToFile()
{
	CAtlFile file;
	if (SUCCEEDED(file.Create(_T("cards.txt"), GENERIC_WRITE, FILE_SHARE_READ, CREATE_ALWAYS)))
	{
		for (int i = 0; i < m_cards.GetSize(); i++)
		{
			CString strLine;
			strLine.Format(_T("%d,%s,%s,%d,%d,%s,%d,%d\n"),
				m_cards[i].id,
				m_cards[i].name,
				m_cards[i].surname,
				m_cards[i].numberButtonGender,
				m_cards[i].numberFamilyStatus,
				m_cards[i].dateTimeValue.Format(_T("%Y-%m-%d")),
				m_cards[i].countChilds,
				m_cards[i].checkAutomobile
				
			);

			//для отладки
			//TRACE(_T("Сформированная строка: %s\n"), strLine);

			CT2A ansiConverter(strLine, CP_ACP);
			const char* ansiLine = ansiConverter;
			//для отладки
			//TRACE("Преобразованная строка: %s\n", ansiLine);

			file.Write(ansiLine, strlen(ansiLine));
		}
	}
}


// При добавлении кнопки свертывания в диалоговое окно нужно воспользоваться приведенным ниже кодом,
//  чтобы нарисовать значок.  Для приложений MFC, использующих модель документов или представлений,
//  это автоматически выполняется рабочей областью.

void CCardfilemfcDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // контекст устройства для рисования

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Выравнивание значка по центру клиентского прямоугольника
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Нарисуйте значок
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
HCURSOR CCardfilemfcDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CCardfilemfcDlg::OnBnClickedButtondelete()
{
	// Получить индекс выбранного элемента
	int selectedIndex = m_listBox.GetCurSel();

	// Проверить, что элемент выбран
	if (selectedIndex != LB_ERR)
	{
		// Отобразить модальное окно с вопросом и кнопками "Да" и "Нет"
		if (AfxMessageBox(_T("Вы действительно хотите удалить элемент?"), MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			// Удалить выбранный элемент
			m_cards.RemoveAt(selectedIndex);
			m_listBox.DeleteString(selectedIndex);

			// Если возможно, выбрать следующий элемент после удаления
			if (selectedIndex < m_listBox.GetCount())
			{
				m_listBox.SetCurSel(selectedIndex);
			}
			else if (selectedIndex > 0) // Иначе выбрать предыдущий элемент
			{
				m_listBox.SetCurSel(selectedIndex - 1);
			}
		}
	}
	UpdateButtonDeleteState();
}


void CCardfilemfcDlg::OnBnClickedButtonadd()
{
	OnBnClickedButtonok();
	paramName = _T("Новая");
	paramSurname = _T("карточка");
	paramChilds = 0;
	UpdateData(FALSE); // обновить поля на те, что в переменных
	m_radioMale.SetCheck(BST_CHECKED);
	m_comboBox.SetCurSel(0);
	paramChilds = 0;
	COleDateTime defaultDate(1970, 1, 1, 0, 0, 0);
	m_dateTimePicker.SetTime(defaultDate);
	//m_listBox.AddString((paramName + ' ' + paramSurname));
	//m_listBox.SubclassDlgItem(IDC_LISTBOX, this);
	CheckRadioButton(IDC_female, IDC_male, IDC_male);
	CButton* pCheckBox = (CButton*)GetDlgItem(IDC_checkcar); // Устанавливаем Check Box "есть автомобиль" в статус 0.
	pCheckBox->SetCheck(0);
	UpdateData(TRUE);
	//m_listBox.AddString((paramName + ' ' + paramSurname));
	AddCard(++counterId, paramName, paramSurname, GetCheckedRadioButton(IDC_female, IDC_male),m_dateTime, numberStatus, paramChilds, checkCar);
	
	UpdateButtonDeleteState();
	SelectLastItemInListBox();
	showInfoPanel();

}

void CCardfilemfcDlg::SelectLastItemInListBox()
{
	int itemCount = m_listBox.GetCount();
	if (itemCount > 0)
	{
		int lastIndex = itemCount - 1;
		m_listBox.SetCurSel(lastIndex);
	}
}


void CCardfilemfcDlg::OnBnClickedButtonsave()
{
	OnBnClickedButtonok();
	SaveCardsToFile();
}

void CCardfilemfcDlg::AddCard(int id, CString& name, CString& surname, int numberButtonGender, COleDateTime& dateTimeValue, int numberFamilyStatus, int countChilds, bool checkAutomobile)
{
	CardInfo card;
	card.id = id;
	card.name = name;
	card.surname = surname;
	card.numberButtonGender = numberButtonGender;
	card.dateTimeValue = dateTimeValue;
	card.numberFamilyStatus = numberFamilyStatus;
	card.countChilds = countChilds;
	card.checkAutomobile = checkAutomobile;
	m_cards.Add(card);
	m_listBox.AddString(card.name + " "+ card.surname); // Отображаем только имя карточки
}


void CCardfilemfcDlg::SaveCurrentSelection()
{

	if (currentSelection >= 0 && currentSelection < m_cards.GetSize())
	{

		// Сохранить изменения для текущего выбранного элемента
		CardInfo& currentCard = m_cards[currentSelection];
		GetDlgItemText(IDC_NAME, currentCard.name);
		GetDlgItemText(IDC_surname, currentCard.surname);
		currentCard.numberButtonGender = GetCheckedRadioButton(IDC_female, IDC_male);
		CDateTimeCtrl* pDateTimeCtrl = (CDateTimeCtrl*)GetDlgItem(IDC_BIRTHDATE);
		pDateTimeCtrl->GetTime(currentCard.dateTimeValue);
		CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_FAMILYSTATUS);
		currentCard.numberFamilyStatus = pComboBox->GetCurSel();
		CButton* pCheckBox = (CButton*)GetDlgItem(IDC_checkcar);
		currentCard.checkAutomobile = (pCheckBox->GetCheck() == BST_CHECKED);
		currentCard.countChilds = m_spinChild.GetPos();

		// Обновить название элемента ListBox
		CString newDisplayName = currentCard.name + _T(' ') + currentCard.surname;
		m_listBox.SetItemData(currentSelection, (DWORD_PTR)&currentCard); 
		m_listBox.DeleteString(currentSelection);
		m_listBox.InsertString(currentSelection, newDisplayName);
	}
	
}


void CCardfilemfcDlg::OnLbnSelchangeListbox()
{
	//UpdateData(TRUE);
	// Сначала сохраните изменения для текущего выбранного элемента
	SaveCurrentSelection();

	// Получите новый выбранный индекс
	int selectedIndex = m_listBox.GetCurSel();
	if (selectedIndex != LB_ERR)
	{
		showInfoPanel();

		CardInfo selectedCard = m_cards[selectedIndex];
		//Имя и фамилия
		SetDlgItemText(IDC_NAME, selectedCard.name);
		SetDlgItemText(IDC_surname, selectedCard.surname);
		//Пол
		CheckRadioButton(IDC_female, IDC_male, selectedCard.numberButtonGender);
		//Дата рождения
		CDateTimeCtrl* pDateTimeCtrl = (CDateTimeCtrl*)GetDlgItem(IDC_BIRTHDATE);
		pDateTimeCtrl->SetTime(selectedCard.dateTimeValue);
		//Семейное положение
		CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_FAMILYSTATUS);
		pComboBox->SetCurSel(selectedCard.numberFamilyStatus);
		//Наличие автомобиля
		m_spinChild.SetPos(selectedCard.countChilds);
		CButton* pCheckBox = (CButton*)GetDlgItem(IDC_checkcar);
		pCheckBox->SetCheck(selectedCard.checkAutomobile ? BST_CHECKED : BST_UNCHECKED);

		// Обновите m_currentSelection
		currentSelection = selectedIndex;
		UpdateData(TRUE);
	}
	else
	{
		// Если выбор недействителен, скроем панель информации
		hideInfoPanel();

		// Установим m_currentSelection в -1
		currentSelection = -1;
	}
}


//Сохранение полей в переменные, если происходит их изменение пользователем:

void CCardfilemfcDlg::OnEnChangeName()
{
	UpdateData(TRUE);
}

void CCardfilemfcDlg::OnEnChangeSurname()
{
	UpdateData(TRUE);
}


void CCardfilemfcDlg::OnDtnDatetimeChangeBirthdate(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	*pResult = 0;
	UpdateData(TRUE);
}

//
void CCardfilemfcDlg::UpdateButtonDeleteState()
{
	// Проверка, пуст ли список ListBox
	if (m_listBox.GetCount() == 0)
	{
		// Сделать кнопку неактивной
		buttonDelete.EnableWindow(FALSE);
	}
	else
	{
		// Сделать кнопку активной
		buttonDelete.EnableWindow(TRUE);
	}
}




void CCardfilemfcDlg::showInfoPanel() 
{
	m_paramName.ShowWindow(SW_SHOW);
	m_paramSurname.ShowWindow(SW_SHOW);
	m_textname.ShowWindow(SW_SHOW);
	m_textsurname.ShowWindow(SW_SHOW);
	m_radioMale.ShowWindow(SW_SHOW);
	m_radioFemale.ShowWindow(SW_SHOW);
	m_Gendertext.ShowWindow(SW_SHOW);
	m_datetext.ShowWindow(SW_SHOW);
	m_FamilyStatustext.ShowWindow(SW_SHOW);
	m_childstext.ShowWindow(SW_SHOW);
	m_checkcar.ShowWindow(SW_SHOW);
	m_comboBox.ShowWindow(SW_SHOW);
	m_dateTimePicker.ShowWindow(SW_SHOW);
	m_spinChild.ShowWindow(SW_SHOW);
	m_Childs.ShowWindow(SW_SHOW);
	m_ButtonOk.ShowWindow(SW_SHOW);

	m_infotext.ShowWindow(SW_HIDE);
};


void CCardfilemfcDlg::hideInfoPanel() 
{
	m_paramName.ShowWindow(SW_HIDE);
	m_paramSurname.ShowWindow(SW_HIDE);
	m_textname.ShowWindow(SW_HIDE);
	m_textsurname.ShowWindow(SW_HIDE);
	m_radioMale.ShowWindow(SW_HIDE);
	m_radioFemale.ShowWindow(SW_HIDE);
	m_Gendertext.ShowWindow(SW_HIDE);
	m_datetext.ShowWindow(SW_HIDE);
	m_FamilyStatustext.ShowWindow(SW_HIDE);
	m_childstext.ShowWindow(SW_HIDE);
	m_checkcar.ShowWindow(SW_HIDE);
	m_comboBox.ShowWindow(SW_HIDE);
	m_dateTimePicker.ShowWindow(SW_HIDE);
	m_spinChild.ShowWindow(SW_HIDE);
	m_Childs.ShowWindow(SW_HIDE);
	m_ButtonOk.ShowWindow(SW_HIDE);

	m_infotext.ShowWindow(SW_SHOW);
};


void CCardfilemfcDlg::OnBnClickedButtonok()
{
	int selectedIndex = m_listBox.GetCurSel();
	if (selectedIndex != LB_ERR)
	{
		UpdateData(TRUE);
		m_cards[selectedIndex].name = paramName;
		m_cards[selectedIndex].surname = paramSurname;
		m_cards[selectedIndex].numberButtonGender = GetCheckedRadioButton(IDC_female, IDC_male);
		m_cards[selectedIndex].numberFamilyStatus = m_comboBox.GetCurSel();
		m_cards[selectedIndex].checkAutomobile = checkCar;
		m_cards[selectedIndex].dateTimeValue = m_dateTime;
		m_cards[selectedIndex].countChilds = paramChilds;

		m_listBox.DeleteString(selectedIndex);
		m_listBox.InsertString(selectedIndex, paramName + ' ' + paramSurname);
		m_listBox.SetCurSel(selectedIndex);
	}

}


void CCardfilemfcDlg::OnEnSetfocuschildcount()
{
	m_spinChild.SetFocus();
}


void CCardfilemfcDlg::OnBnClickedButtonexit()
{
	bool isEqual = AreCardsEqual(m_cards, m_cardsReserv);
	if (!isEqual) {
		if (AfxMessageBox(_T("Вы хотите сохранить изменения?"), MB_YESNO | MB_ICONQUESTION) == IDYES)
		{

			OnBnClickedButtonsave();
		}
	}

	OnOK();
}

bool CCardfilemfcDlg::AreCardsEqual(const CArray<CardInfo, CardInfo&>& array1, const CArray<CardInfo, CardInfo&>& array2)
{
	if (array1.GetSize() != array2.GetSize()) {
		return false;
	}

	for (int i = 0; i < array1.GetSize(); ++i) {
		if (!(array1[i] == array2[i])) {
			return false;
		}
	}

	return true;
}