1.ֻ��Ҫ��UIDlgBuilder.cpp�����һ����룺        
  if( _tcscmp(pstrClass, _T("Button")) == 0 )                 pControl = new CButtonUI;
  �޸�Ϊif( _tcscmp(pstrClass, _T("Button")) == 0 )                 pControl = new CFadeButtonUI;

2.��Ƥ���������У�ѡ��Button��ʵ����ʹ�õ���CFadeButtonUI

3.CFadeButtonUI�Զ��ڽ�������˳�����״̬ʱ��ȡm_sNormalImage��m_sHotImage���Բ��ϱ仯������ռ�����ﵽ��̬�Ľ���Ч����

4.�ɵ���CFadeButtonUI::SetFadeEffect�ӿ��������Ƿ�Ҫ����Ч����


�˴����ڡ�����-��ţ��QQ��153139715�����Ĵ���������޸ġ�
�޸��ˣ����ⲻ�⣨QQ:185025899��