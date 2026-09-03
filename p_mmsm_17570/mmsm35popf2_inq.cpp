/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-05-25
Description: 分切实绩待录入查询
***********************************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



/* ***** 静态函数申明 ***** */


/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
/// CC实绩查询
/// <para>
/// 
///
/// </para>
/// <para></para>
/// <para>    分切录入时查询数据，并返回待分切的子坯数据     </para>
/// </summary>
/// <param name="">                      </param>
/// <param name="">                    </param>
/// <returns></returns>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(mmsm35popf2_inq)

int f_mmsm35popf2_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_t01 = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	int		TotalRecordCount = 0;
	CString v_mat_no = "";
	
	CString v_heat_no = "";
	CDecimal v_cut_num = 0;
	CString v_proc_div = "";// F6   F7

	CString ch_start_time_f = "";
	CString ch_start_time_t = "";
	CString v_batch = "";//获取批次号
	CDecimal v_batch_seq = 0;//批次号最后两位序号
	CString v_print_no = "";//喷印号
	CString v_slab_no = "";//板坯号
	CString v_in_batch = "";//入口批次号
	CDecimal v_in_mat_len = 0;//入口长度
	CDecimal v_in_mat_wt = 0;//入口重量
	CDecimal v_mat_width = 0;//入口宽度
	CDecimal v_mat_thick = 0;//入口厚度
	CDecimal v_cutafter_len = 0;//切后长度
	CDecimal v_cutafter_wt = 0;//切后重量
	CDecimal v_cutFei_len = 0;//切废长度
	CDecimal v_cutFei_wt = 0;//切废重量

	//苹果钢
	int t = 0;
	int temp = 0;
	CString no = ""; //苹果钢材料号第11、12位


	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsm35("TMMSM35");
	CModel tmmsm01("TMMSM01");

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_t01(conn);


	try
	{
		try
		{//获取前台DEV控件传入的分页信息
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		}
		catch (CException& ce)
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize = 1000;
		}
		if (bcls_rec->Tables[0].Rows[0]["PROC_DIV"].ToString().Trim() != "")
			v_proc_div = bcls_rec->Tables[0].Rows[0]["PROC_DIV"].ToString().Trim();
		if (bcls_rec->Tables[0].Rows[0]["IN_MAT_NO"].ToString().Trim() != "")
			v_mat_no = bcls_rec->Tables[0].Rows[0]["IN_MAT_NO"].ToString().Trim();
		if (bcls_rec->Tables[0].Rows[0]["IN_BATCH"].ToString().Trim() != "")
			v_in_batch = bcls_rec->Tables[0].Rows[0]["IN_BATCH"].ToString().Trim();
		if (bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim() != "")
			v_heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		if (bcls_rec->Tables[0].Rows[0]["IN_PRINT_NO"].ToString().Trim() != "")
			v_print_no = bcls_rec->Tables[0].Rows[0]["IN_PRINT_NO"].ToString().Trim();
		if (bcls_rec->Tables[0].Rows[0]["IN_SLAB_NO"].ToString().Trim() != "")
			v_slab_no = bcls_rec->Tables[0].Rows[0]["IN_SLAB_NO"].ToString().Trim();
		if (bcls_rec->Tables[0].Rows[0]["CUT_NUM"].ToDecimal() != 0)
			v_cut_num = bcls_rec->Tables[0].Rows[0]["CUT_NUM"].ToDecimal();
		if (v_proc_div == "F7")
		{
			if (bcls_rec->Tables[0].Rows[0]["IN_MAT_LEN"].ToDecimal() != 0)
				v_in_mat_len = bcls_rec->Tables[0].Rows[0]["IN_MAT_LEN"].ToDecimal();
			if (bcls_rec->Tables[0].Rows[0]["IN_MAT_WT"].ToDecimal() != 0)
				v_in_mat_wt = bcls_rec->Tables[0].Rows[0]["IN_MAT_WT"].ToDecimal();
			if (bcls_rec->Tables[0].Rows[0]["MAT_WIDTH"].ToDecimal() != 0)
				v_mat_width = bcls_rec->Tables[0].Rows[0]["MAT_WIDTH"].ToDecimal();
			if (bcls_rec->Tables[0].Rows[0]["MAT_THICK"].ToDecimal() != 0)
				v_mat_thick = bcls_rec->Tables[0].Rows[0]["MAT_THICK"].ToDecimal();
		}
		
		if (v_slab_no.Trim() == "")
		{
			CFormattable arguments[] = { v_mat_no };
			CMessageFormat::Format(s.msg, "材料{0}的板坯号没有数据，请确认后重新操作！", arguments, 1);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		Log::Info("", __FUNCTION__, "v_cut_num  =[{0}] v_mat_no = [{1}] v_heat_no= [{2}]", v_cut_num, v_mat_no, v_heat_no);

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			//根据熔炼号查找最大批次号，
			//先查35表，若35表没有数据，则查01表，若01表没有数据，表示第一次分切，从31开始  mfj  20240612
			sqlstr = " SELECT  BATCH,SUBSTR(BATCH,9,2) FROM TMMSM35 WHERE HEAT_NO = '" + v_heat_no + "' AND SUBSTR(BATCH,9,2) > '30' "
				" AND SUBSTR(BATCH, 9, 2) <> '99' AND SUBSTR(BATCH, 9, 2) <> 'ZZ' AND SUBSTR(BATCH, 9, 2) <> 'AA'   order by BATCH desc ";
			
			sqlstr_count = " SELECT COUNT(1) "
				"   FROM TMMSM35 "
				"  WHERE INITIAL_BATCH LIKE '{0}%' "
				;
			break;
		}
		Log::Info("", __FUNCTION__, "sqlstr  =[{0}]", sqlstr);

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())//若查到，则表示该批号已经产出，后续按此继续增加序号即可
		{
			v_batch = cmd_inq.GetString(1);
			v_batch_seq = cmd_inq.GetDecimal(2);
			Log::Info("", __FUNCTION__, "v_batch_seq  =[{0}]", v_batch_seq);
		}
		else
		{
			//根据熔炼号查找最大批次号，
			sqlstr_t01 = " SELECT  BATCH,SUBSTR(BATCH,9,2) FROM VMMSM01 WHERE HEAT_NO = '" + v_heat_no + "' AND SUBSTR(BATCH,9,2) > '30' "
				" AND SUBSTR(BATCH, 9, 2) <> '99' AND SUBSTR(BATCH, 9, 2) <> 'ZZ' AND SUBSTR(BATCH, 9, 2) <> 'AA'   order by BATCH desc ";
			cmd_inq_t01.SetCommandText(sqlstr_t01);
			cmd_inq_t01.ExecuteReader();
			if (cmd_inq_t01.Read())//若查到，则表示该批号已经产出，后续按此继续增加序号即可
			{
				v_batch = cmd_inq_t01.GetString(1);
				v_batch_seq = cmd_inq_t01.GetDecimal(2);
			}
			else//没有查到说明是该炉第一次分切的，此时批次号从31开始
			{
				v_batch_seq = 30;
			}
			cmd_inq_t01.Close();
		}
		cmd_inq.Close();
		Log::Info("", __FUNCTION__, "v_batch_seq 111 =[{0}]", v_batch_seq);
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "BATCH");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "PRINT_NO");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "SLAB_NO");

		//苹果钢   暂时定为  当分切数大于30支的时候是苹果钢。  mfj  20240219  
		//  20240307  修改 F7按钮只作为苹果钢使用  mfj
		if (v_proc_div == "F7")
		{
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "MAT_NO");
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
			bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "MAT_THICK");
			bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "MAT_WIDTH");
			bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "MAT_LEN");
			bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "MAT_WT");
			bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "IN_MAT_LEN");
			bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "IN_MAT_WT");
			bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "MAT_NUM");

			v_cutafter_len = (v_in_mat_len / v_cut_num).Floor();
			v_cutafter_wt = (((v_in_mat_wt * 1000) / v_cut_num).Floor()) / 1000;//重量先乘以1000，再除以切割支数，再向下取整后除以1000转换成吨单位；

			v_cutFei_len = v_in_mat_len - (v_cutafter_len * v_cut_num);
			v_cutFei_wt = v_in_mat_wt - (v_cutafter_wt * v_cut_num);


			for (int i = 1; i <= v_cut_num; i++)
			{
				cmd_inq.SetCommandText(sqlstr_count);
				t = cmd_inq.ExecuteScalar().ToInt32();
				Log::Info("", __FUNCTION__, "t  =[{0}]", t);

				temp = t + 11 + (((t + 11 + (t / 10)) / 10) - 1);
				temp = temp + (i-1);				
				Log::Info("", __FUNCTION__, "temp  =[{0}]", temp);
				no = to_string(temp);

				bcls_ret->Tables[0].Rows.Add();
				bcls_ret->Tables[0].Rows[i - 1]["MAT_NO"] = v_mat_no + no;


				if (i == 1)
				{
					if (v_in_batch != "")
						bcls_ret->Tables[0].Rows[i - 1]["BATCH"] = v_in_batch;
					else
						bcls_ret->Tables[0].Rows[i - 1]["BATCH"] = v_mat_no;
					bcls_ret->Tables[0].Rows[i - 1]["PRINT_NO"] = v_print_no;
					bcls_ret->Tables[0].Rows[i - 1]["SLAB_NO"] = v_slab_no;
				}
				else
				{
					bcls_ret->Tables[0].Rows[i - 1]["BATCH"] = v_heat_no + (v_batch_seq + i - 1).ToString();
					//bcls_ret->Tables[0].Rows[i - 1]["PRINT_NO"] = v_print_no.Substring(0, 15) + (v_batch_seq + i - 1).ToString() + v_print_no.Substring(17);
					bcls_ret->Tables[0].Rows[i - 1]["SLAB_NO"] = v_slab_no.SubstringNE(0, 15) + (v_batch_seq + i - 1).ToString() + v_slab_no.SubstringNE(17);
				}
				bcls_ret->Tables[0].Rows[i - 1]["HEAT_NO"] = v_heat_no;
				bcls_ret->Tables[0].Rows[i - 1]["MAT_THICK"] = v_mat_thick;
				bcls_ret->Tables[0].Rows[i - 1]["MAT_WIDTH"] = v_mat_width;
				bcls_ret->Tables[0].Rows[i - 1]["MAT_LEN"] = v_cutafter_len;
				bcls_ret->Tables[0].Rows[i - 1]["MAT_WT"] = v_cutafter_wt;
				bcls_ret->Tables[0].Rows[i - 1]["IN_MAT_LEN"] = v_cutFei_len;
				bcls_ret->Tables[0].Rows[i - 1]["IN_MAT_WT"] = v_cutFei_wt;
				bcls_ret->Tables[0].Rows[i - 1]["MAT_NUM"] = 1;
			}
		}
		else if (v_proc_div == "F7")
		{
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "MAT_NO");
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
			bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "MAT_THICK");
			bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "MAT_WIDTH");
			bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "MAT_LEN");
			bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "MAT_WT");
			bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "IN_MAT_LEN");
			bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "IN_MAT_WT");
			bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "MAT_NUM");

			v_cutafter_len = (v_in_mat_len / v_cut_num).Floor();
			v_cutafter_wt = (((v_in_mat_wt * 1000) / v_cut_num).Floor()) / 1000;//重量先乘以1000，再除以切割支数，再向下取整后除以1000转换成吨单位；

			v_cutFei_len = v_in_mat_len - (v_cutafter_len * v_cut_num);
			v_cutFei_wt = v_in_mat_wt - (v_cutafter_wt * v_cut_num);

			for (int i = 1; i <= v_cut_num; i++)
			{
				bcls_ret->Tables[0].Rows.Add();
				if (i > 9)
				{
					bcls_ret->Tables[0].Rows[i - 1]["MAT_NO"] = v_mat_no + to_string(i);
				}
				else
				{
					bcls_ret->Tables[0].Rows[i - 1]["MAT_NO"] = v_mat_no + to_string(i) + "0";
				}
				
				if (i == 1)
				{
					if (v_in_batch != "")
						bcls_ret->Tables[0].Rows[i - 1]["BATCH"] = v_in_batch;
					else
						bcls_ret->Tables[0].Rows[i - 1]["BATCH"] = v_mat_no;
					bcls_ret->Tables[0].Rows[i - 1]["PRINT_NO"] = v_print_no;
					bcls_ret->Tables[0].Rows[i - 1]["SLAB_NO"] = v_slab_no;
				}
				else
				{
					bcls_ret->Tables[0].Rows[i - 1]["BATCH"] = v_heat_no + (v_batch_seq + i - 1).ToString();
					//bcls_ret->Tables[0].Rows[i - 1]["PRINT_NO"] = v_print_no.Substring(0, 15) + (v_batch_seq + i - 1).ToString() + v_print_no.Substring(17);
					bcls_ret->Tables[0].Rows[i - 1]["SLAB_NO"] = v_slab_no.SubstringNE(0, 15) + (v_batch_seq + i - 1).ToString() + v_slab_no.SubstringNE(17);
				}
				bcls_ret->Tables[0].Rows[i - 1]["HEAT_NO"] = v_heat_no;
				bcls_ret->Tables[0].Rows[i - 1]["MAT_THICK"] = v_mat_thick;
				bcls_ret->Tables[0].Rows[i - 1]["MAT_WIDTH"] = v_mat_width;
				bcls_ret->Tables[0].Rows[i - 1]["MAT_LEN"] = v_cutafter_len;
				bcls_ret->Tables[0].Rows[i - 1]["MAT_WT"] = v_cutafter_wt;
				bcls_ret->Tables[0].Rows[i - 1]["IN_MAT_LEN"] = v_cutFei_len;
				bcls_ret->Tables[0].Rows[i - 1]["IN_MAT_WT"] = v_cutFei_wt;
				bcls_ret->Tables[0].Rows[i - 1]["MAT_NUM"] = 1;
			}
		}
		else if (v_proc_div == "F6")
		{
			for (int i = 1; i <= v_cut_num; i++)
			{
				bcls_ret->Tables[0].Rows.Add();
				if (i == 1)
				{
					if (v_in_batch != "")
						bcls_ret->Tables[0].Rows[i - 1]["BATCH"] = v_in_batch;
					else
						bcls_ret->Tables[0].Rows[i - 1]["BATCH"] = v_mat_no;
					bcls_ret->Tables[0].Rows[i - 1]["PRINT_NO"] = v_print_no;
					bcls_ret->Tables[0].Rows[i - 1]["SLAB_NO"] = v_slab_no;
				}
				else
				{
					bcls_ret->Tables[0].Rows[i - 1]["BATCH"] = v_heat_no + (v_batch_seq + i - 1).ToString();
					//bcls_ret->Tables[0].Rows[i - 1]["PRINT_NO"] = v_print_no.Substring(0, 15) + (v_batch_seq + i - 1).ToString() + v_print_no.Substring(17);
					bcls_ret->Tables[0].Rows[i - 1]["SLAB_NO"] = v_slab_no.SubstringNE(0, 15) + (v_batch_seq + i - 1).ToString() + v_slab_no.SubstringNE(17);
				}
			}
		}


	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}


	return doFlag;

}
