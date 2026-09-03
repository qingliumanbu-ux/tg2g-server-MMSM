/*************************************************
Copyright:Baosight Software LTD.co Copyright (c) 2010
Author:闫向东
Date:2021-02-04
Version:1.0
Description: 表实体对象字段超长检测
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
#include "epex.h"


int f_tableObjectCheck9999(ITableObject2& obj)
{
	CTracer log(__FUNCTION__);

	//++++++++++++++++++字段超长检测开始++++++++++++++++++++++++

	Log::Trace("", __FUNCTION__, "++++++++++++++++++字段超长检测开始++++++++++++++++++++++++");

	int do_flag = 0;
	int v_fd = obj.GetFields().get_Count();
	EIClass tmp_blk;
	tmp_blk.Tables[0].Columns.Add(obj);
	obj.MergeTo(tmp_blk.Tables[0]);
	for (int i = 0; i < obj.GetFields().get_Count(); i++)
	{
		CString col_name = obj.GetFields()[i].ColumnName;
		int fd_len = obj.GetFields()[i].Lengh;
		int va_len = tmp_blk.Tables[0].Rows[0][col_name].ToString().GetLength();

		if (obj.GetFields()[i].ColumnType == DsType::DT_STRING)
		{

			/*Log::Trace("", __FUNCTION__, "ColumnName: [{0}] 是字符型,定义长度 :[{1}],值 = [{2}],值长度 = [{3}]"
			, col_name, fd_len
			, tmp_blk.Tables[0].Rows[0][col_name].ToString()
			, va_len);*/

			if (va_len > fd_len)
			{
				Log::Trace("", __FUNCTION__, "！！！！检测到字符型字段: [{0}] 超长,定义长度 :[{1}],值 = [{2}],值长度 = [{3}]"
					, col_name, fd_len
					, tmp_blk.Tables[0].Rows[0][col_name].ToString()
					, va_len);
				do_flag = -1;
			}

		}
		else if (obj.GetFields()[i].ColumnType == DsType::DT_DECIMAL)
		{
			/*va_len = tmp_blk.Tables[0].Rows[0][col_name].ToString().GetLength();
			Log::Trace("", __FUNCTION__, "ColumnName: [{0}] 是数字型,定义长度 :[{1}],值 = [{2}],值长度 = [{3}]"
			, col_name, obj.GetFields()[i].Precision
			, tmp_blk.Tables[0].Rows[0][col_name].ToDecimal()
			, va_len);

			Log::Trace("", __FUNCTION__, "####  小数位: [{0}] ", obj.GetFields()[i].Scale);*/
			if (tmp_blk.Tables[0].Rows[0][col_name].ToDecimal() == 0) continue;

			fd_len = obj.GetFields()[i].Precision;	//数字型的定义长度即为 精度

			int fd_len_aa = obj.GetFields()[i].Precision - obj.GetFields()[i].Scale;	//整数位定义长度
			int fd_len_bb = obj.GetFields()[i].Scale;										//小数位定义长度
			int va_len_aa = tmp_blk.Tables[0].Rows[0][col_name].ToString().Find(".", 0);		//整数位值长度
			if (va_len_aa < 0)
			{
				va_len_aa = va_len;	//无小数点，整数位值长度即为值长度
			}
			else
			{
				va_len = va_len - 1;	//减去1位小数点
			}

			if (fd_len_bb > 0)		//有小数位													//小数位值长度
			{

				int va_len_bb = va_len - va_len_aa;
				if (va_len_aa > fd_len_aa)
				{
					Log::Trace("", __FUNCTION__, "??????检测到数字型字段: [{0}]整数位超长,定义长度 :[{1},{2}],值 = [{3}],值长度 = [{4},{5}]"
						, col_name, obj.GetFields()[i].Precision
						, obj.GetFields()[i].Scale
						, tmp_blk.Tables[0].Rows[0][col_name].ToString()
						, va_len, va_len_bb);
					do_flag = -1;
				}

				if (va_len_bb > fd_len_bb)
				{
					Log::Trace("", __FUNCTION__, "?????检测到数字型字段: [{0}]小数位超长,定义长度 :[{1},{2}],值 = [{3}],值长度 = [{4},{5}]"
						, col_name, obj.GetFields()[i].Precision
						, obj.GetFields()[i].Scale
						, tmp_blk.Tables[0].Rows[0][col_name].ToString()
						, va_len, va_len_bb);
					do_flag = -1;
				}
			}
			else
			{	//没有小数位，即为整型，直接比整长度
				if (va_len > fd_len)
				{
					Log::Trace("", __FUNCTION__, "?????检测到数字型字段: [{0}] 超长,定义长度 :[{1}],小数位: [{2}],值 = [{3}],值长度 = [{4}]"
						, col_name, obj.GetFields()[i].Precision
						, obj.GetFields()[i].Scale
						, tmp_blk.Tables[0].Rows[0][col_name].ToString()
						, va_len);
					do_flag = -1;

				}
			}
		}

	}

	Log::Trace("", __FUNCTION__, "++++++++++++++++++字段超长检测 END ++++++++++++++++++++++++");
	if (do_flag < 0)
	{
		Log::Trace("", __FUNCTION__, "@_@[{0}]小朋友，检查到表实体[{1}]有超长的字段哦！！", s.username, obj.GetTableName());
		//throw CApplicationException(-1, s.msg, log.Location);
		return do_flag;
	}
	Log::Trace("", __FUNCTION__, "@_@什么都没发现！");
	return do_flag;
}