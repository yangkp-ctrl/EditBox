#pragma once
#include"CText.h"
#include"Cursor.h"
#include<Windows.h>
//记录操作信息
#define RD_DELETE 0x00000001
#define RD_INSERT 0x00000010
#define RD_RETURN 0x00000100
#define RD_CHOOSE 0x00001000
struct Record
{
	Record(UINT flag);
	~Record();
	void ReDo(CText* p);				//撤销
	void Save_Delete_Data(CText* pc,Position first,Position last);	//删除前保留删除信息
	void Save_Insert_Data(Position first,Position last);			//保存插入信息
	void Set_Choose_Data(Position first, Position last);			//设置选中信息
	void Clear_Choose_Data();
	Position start;						//选中开始
	Position end;						//选中结束
	CText* pText;						//绑定的文本句柄
	CText* pData;						//保存信息
	UINT ACT;							//上一步的动作
};


/*
应该保存的状态
删除
插入
回车
选中信息
*/


/*
RD_RETURN ： 回车
start 记录按下回车的位置 (光标在start后按下回车)
eg. 1   1234|56    start={1,4}
result:
	1	1234
	2	56

	撤销后恢复原状，statr仍记录光标前的字符位置
*/

/*
RD_INSERT
start 记录插入的起始位置
end   记录插入的终止位置
即 插入的内容位[start,end]
*/

/*
RD_CHOOSE
start 和 end 记录了光标选中信息
注：RD_CHOOSE只记录用户的选中信息，撤销时应当忽略此步
*/

/*
RD_DELETE
start 记录了删除的起点
end   记录了删除的终点
pData 记录了删除的内容
*/