1.只需要把UIDlgBuilder.cpp里面的一句代码：        
  if( _tcscmp(pstrClass, _T("Button")) == 0 )                 pControl = new CButtonUI;
  修改为if( _tcscmp(pstrClass, _T("Button")) == 0 )                 pControl = new CFadeButtonUI;

2.在皮肤编译器中，选择Button，实际上使用的是CFadeButtonUI

3.CFadeButtonUI自动在进入或者退出焦点状态时，取m_sNormalImage和m_sHotImage，以不断变化的两者占比来达到动态的渐变效果。

4.可调用CFadeButtonUI::SetFadeEffect接口来设置是否要渐变效果。


此代码在“杭州-阿牛（QQ：153139715）”的代码基础上修改。
修改人：阳光不锈（QQ:185025899）