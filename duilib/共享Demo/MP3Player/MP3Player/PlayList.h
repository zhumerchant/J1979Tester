#ifndef UIplaylists_H
#define UIplaylists_H

#include "UIListCommonDefine.h"

namespace DuiLib
{
struct PlayListItemInfo
{
	bool folder;   //是否文件夹
	bool empty;    //文件夹是否为空
	bool playing;  //是否正在播放
	CDuiString id;
	CDuiString logo;  //图标
	CDuiString file_name;  //歌曲名
	CDuiString full_filename;  //描述
};


class CPlayList : public CListUI
{
public:
	enum {SCROLL_TIMERID = 10};

	CPlayList(CPaintManagerUI& paint_manager);

	~CPlayList();

	
#if _MSC_VER <=1200
    typedef CListUI __super;
#endif


	bool Add(CControlUI* pControl);

	bool AddAt(CControlUI* pControl, int iIndex);

	bool Remove(CControlUI* pControl);

	bool RemoveAt(int iIndex);

	void RemoveAll();

	void DoEvent(TEventUI& event);

	Node* GetRoot();

	Node* AddNode(const PlayListItemInfo& item, Node* parent = NULL);

	bool RemoveNode(Node* node);

	void SetChildVisible(Node* node, bool visible);

	bool CanExpand(Node* node) const;

	bool SelectItem(int iIndex, bool bTakeFocus = false);

	void InvalidateNodes(Node *node);

private:
	Node*	root_node_;
	LONG	delay_deltaY_;
	DWORD	delay_number_;
	DWORD	delay_left_;
	CDuiRect	text_padding_;
	int level_text_start_pos_;
	CDuiString level_expand_image_;
	CDuiString level_collapse_image_;
	CPaintManagerUI& paint_manager_;

    CDialogBuilder m_dlgBuilder;
};

} // DuiLib

#endif // UIplaylists_HPP