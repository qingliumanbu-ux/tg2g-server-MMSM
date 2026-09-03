/*******************************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-07-04
Description: 获取材料号
remark:本函数目前暂不考虑按批管理
***********************************************************************/
/***** C/C++ 的标准头文件部分 *****/
// New Include

//框架公用头文件，勿删
#include "stdafx.h"
/***** C++ 的业务头文件部分 *****/


//外部函数声明
BM2_FUNCTION_EXPORT
int f_mmsm_mat_no_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	/****** 定义函数名称 ***** */
	CString FunctionEname = "f_mmsm_matno_catch";                //定义函数英文名称  
	CString FunctionCname = "获取材料号";              //定义函数中文名称
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	/****** 自定义变量 ***** */
	int doFlag = 0;
	int ret = 0;
	int blkNum = 0;

	CString c_datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");          //当前时间
	CString v_func_id = "";
	CString old_mat_no = "";//传入的材料号 
	CString v_mat_no = ""; //生成的最新的材料号
	CString v_heat_no = "";
	CString v_slab_type = "";
	CString v_code_class = "";
	CString c_mat_no = "";
	CString cut_str = "";
	CDecimal n_count = 0;             //校验是否已存在该记录
	CDecimal v_mat_id_max = 0;        //记录本次调用中，传入记录中的锭坯序列号最大值
	int i_cut_len = 0;
	CString c_mat_id_max = "";
	CDecimal i_mat_id_max = 0;
	CString cut_seqid = "";
	int fetchRowCount = 0;
	CString sqlstr = "";
	CString sqlstr_mat_no = "";
	EIClass inBlock;          //材料主档信息处理用
	//EIClass outBlock;
	CString trace_flag = "N";    //N- 不显示履历  Y-反之 

	char mat_11;//第十一位字符
	char mat_12;//第十二位字符
	char mat_new_11;//新材料号第十一位
	char mat_new_12;//新材料号第十二位


	CModel tmmsm01("TMMSM01");
	CModel hmmsm01("HMMSM01");

	CString heatNo = "";
	CString newMatNo = "";
	CString newMatNoSeq = "";
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_mat_no(conn);

	try
	{
		bcls_ret->Tables[0].Clear();
		bcls_ret->Tables[0].Copy(bcls_rec->Tables[0]);

		if (bcls_rec->Tables[0].Columns.Contains("TRACE_FLAG"))
		{
			trace_flag = bcls_rec->Tables[0].Rows[0]["TRACE_FLAG"].ToString();
		}
		Log::Trace("", __FUNCTION__, "履历开关：[{0}]", trace_flag);
		if (!bcls_ret->Tables[0].Columns.Contains("MAT_NO"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "MAT_NO");//实物材料号
		}
		if (!bcls_rec->Tables[0].Columns.Contains("MAT_NO"))
		{
			bcls_rec->Tables[0].Columns.Add(DT_STRING, "MAT_NO");//实物材料号
		}

		if (!bcls_ret->Tables[0].Columns.Contains("MAT_CUT_SEQID"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "MAT_CUT_SEQID");//实物材料序号
		}
		if (!bcls_rec->Tables[0].Columns.Contains("MAT_CUT_SEQID"))
		{
			bcls_rec->Tables[0].Columns.Add(DT_STRING, "MAT_CUT_SEQID");//实物材料序号
		}

		if (!bcls_ret->Tables[0].Columns.Contains("MAT_TUBE"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "MAT_TUBE");//材料根数
		}
		if (!bcls_ret->Tables[0].Columns.Contains("FIX_SLAB_NUM"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "FIX_SLAB_NUM");//定尺板坯块数
		}

		if (!bcls_rec->Tables[0].Columns.Contains("FUNC_ID"))
		{
			//不传配置代码，以SLAB_TYPE来判断
			if (!bcls_rec->Tables[0].Columns.Contains("SLAB_TYPE"))
			{
				sprintf(s.msg, "传入参数FUNC_ID与SLAB_TYPE都不存在。");
				//strcpy(s.sysmsg,s.msg);
				throw CApplicationException(-1, s.msg, log.Location);

			}
			v_slab_type = bcls_rec->Tables[0].Rows[0]["SLAB_TYPE"].ToString();
		}
		/* 获取输入参数*/

		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			if (trace_flag.Trim() == "Y")Log::Trace("", __FUNCTION__, "第[{0}]次处理", i);
			//判断传入数据中是否存在最大序号
			int random_len = 0;

			//当材料号为空时，为33切断获取材料号，当不为空时，为分切和苹果切
			if (bcls_rec->Tables[0].Rows[i]["MAT_NO"].ToString().Trim() == "")
			{
				heatNo = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
				Log::Trace("", __FUNCTION__, "heatNo	= [{0}]", heatNo);
				// MAT_NO  = 熔炼号+材料顺序号+00 
				v_mat_no = heatNo;

				CString v_strand_no = bcls_rec->Tables[0].Rows[i]["STRAND_NO"].ToString().Trim();//流号

				//对于双流情况，不允许批量新增，不然生成的材料号数据不对，C,E都是单数材料号，D,F是双数材料号，批量新增时，只能选择一种流号，故双流不允许批量新增。 mfj  20240229
				if (bcls_rec->Tables[0].Rows.get_Count() > 1)
				{
					if (v_strand_no == "C" || v_strand_no == "D" || v_strand_no == "E" || v_strand_no == "F")
					{
						sprintf(s.msg, "双流不允许批量新增！。");
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}

				//材料号生成 取自喷印号  头坯尾坯时会有00,99，AA,ZZ的情况  太钢定制  mfj   20231228  
				if (bcls_rec->Tables[0].Rows[i]["SLAB_PLACE_CODE"].ToString().Trim() == "B"
					|| bcls_rec->Tables[0].Rows[i]["SLAB_PLACE_CODE"].ToString().Trim() == "T")//头坯或尾坯
				{

					CString cut_no = "";//切割顺序号

					if (bcls_rec->Tables[0].Rows[i]["SLAB_PLACE_CODE"].ToString().Trim() == "B")//头坯
					{
						//当为头坯单流时，顺序号为00 ,双流为AA
						if (v_strand_no == "Z" || v_strand_no == "A" || v_strand_no == "B")
						{
							cut_no = "00";
						}
						else if (v_strand_no == "C" || v_strand_no == "D" || v_strand_no == "E" || v_strand_no == "F")
						{
							cut_no = "AA";
						}
					}
					else if (bcls_rec->Tables[0].Rows[i]["SLAB_PLACE_CODE"].ToString().Trim() == "T")//尾坯
					{
						//当为尾坯单流时，顺序号为99 ,双流为ZZ
						if (v_strand_no == "Z" || v_strand_no == "A" || v_strand_no == "B")
						{
							cut_no = "99";
						}
						else if (v_strand_no == "C" || v_strand_no == "D" || v_strand_no == "E" || v_strand_no == "F")
						{
							cut_no = "ZZ";
						}
					}
					v_mat_no = v_mat_no + cut_no;
				}
				else
				{
					//双流材料号，CE 是单数，DF是双数
					if (v_strand_no == "Z" || v_strand_no == "A" || v_strand_no == "B"){
						cmd_inq.SetCommandText("SELECT NVL(MAX(SUBSTR( MAT_NO, LENGTH(MAT_NO) - 1 , 2 )),0) FROM TMMSM33 WHERE HEAT_NO = @heatNo  AND SUBSTR(MAT_NO,9,2) <> 'AA' AND SUBSTR(MAT_NO,9,2) <> '00' AND SUBSTR(MAT_NO,9,2) <> '99' AND SUBSTR(MAT_NO,9,2) <> 'ZZ' ");
						cmd_inq.Parameters.Set("heatNo", heatNo);
						cmd_inq.ExecuteReader();
						if (cmd_inq.Read())
						{
							if (cmd_inq.GetString(1).Trim() != "")
							{
								/*if (cmd_inq.GetString(1).Trim() == "00")*/
								if ((CDecimal::Parse(cmd_inq.GetString(1).Trim()) + i + 1).ToString().Trim().GetLength() == 1)
								{
									Log::Trace("", __FUNCTION__, "qqq	= [{0}]", (CDecimal::Parse(cmd_inq.GetString(1).Trim()) + i + 1).ToString().Trim());
									v_mat_no = v_mat_no + "0" + (CDecimal::Parse(cmd_inq.GetString(1).Trim()) + i + 1).ToString().Trim();
								}
								else
								{
									v_mat_no = v_mat_no + (CDecimal::Parse(cmd_inq.GetString(1).Trim()) + i + 1).ToString().Trim();
								}
							}
							else//当没有找到的时候表示为第二只坯子(当前获取的可能为AA,00)
							{
								cmd_inq_mat_no.SetCommandText("SELECT NVL(MAX(SUBSTR( MAT_NO, LENGTH(MAT_NO) - 1 , 2 )),0) FROM TMMSM33 WHERE HEAT_NO = @heatNo ");
								cmd_inq_mat_no.Parameters.Set("heatNo", heatNo);
								cmd_inq_mat_no.ExecuteReader();
								if (cmd_inq_mat_no.Read())
								{
									if (cmd_inq_mat_no.GetString(1).Trim() != "")
									{
										v_mat_no = v_mat_no + "01";
									}
								}
								cmd_inq_mat_no.Close();
							}
						}
						cmd_inq.Close();
					}
					else if (v_strand_no == "C" || v_strand_no == "D" || v_strand_no == "E" || v_strand_no == "F")
					{
						cmd_inq.SetCommandText("SELECT NVL(MAX(SUBSTR( MAT_NO, LENGTH(MAT_NO) - 1 , 2 )),0) FROM TMMSM33 WHERE HEAT_NO = @heatNo  AND STRAND_NO ='" + v_strand_no + "' "
							" AND SUBSTR(MAT_NO,9,2) <> 'AA' AND SUBSTR(MAT_NO,9,2) <> '00' AND SUBSTR(MAT_NO,9,2) <> '99' AND SUBSTR(MAT_NO,9,2) <> 'ZZ' ");
						cmd_inq.Parameters.Set("heatNo", heatNo);
						cmd_inq.ExecuteReader();
						if (cmd_inq.Read())
						{
							if (cmd_inq.GetString(1).Trim() != "")
							{
								//当查询到时，表示查到的是01或02以上的数据，此时所有根据流号查询的数据都+2，获取同流的数据
								/*if (cmd_inq.GetString(1).Trim() == "00")*/
								if ((CDecimal::Parse(cmd_inq.GetString(1).Trim()) + i + 2).ToString().Trim().GetLength() == 1)
								{
									if ((v_strand_no == "C" || v_strand_no == "E") && cmd_inq.GetString(1).Trim() == "0")
									{
										v_mat_no = v_mat_no + "01";
									}
									else
									{
										Log::Trace("", __FUNCTION__, "qqq	= [{0}]", (CDecimal::Parse(cmd_inq.GetString(1).Trim()) + i + 2).ToString().Trim());
										v_mat_no = v_mat_no + "0" + (CDecimal::Parse(cmd_inq.GetString(1).Trim()) + i + 2).ToString().Trim();
									}
									
								}
								else
								{
									v_mat_no = v_mat_no + (CDecimal::Parse(cmd_inq.GetString(1).Trim()) + i + 2).ToString().Trim();
								}
							}
							else//当没有找到的时候表示为第二只坯子(当前获取的可能为AA,00)   //这里不一定会走，因为没有查到时会返回0
							{
								cmd_inq_mat_no.SetCommandText("SELECT NVL(MAX(SUBSTR( MAT_NO, LENGTH(MAT_NO) - 1 , 2 )),0) FROM TMMSM33 WHERE HEAT_NO = @heatNo ");
								cmd_inq_mat_no.Parameters.Set("heatNo", heatNo);
								cmd_inq_mat_no.ExecuteReader();
								if (cmd_inq_mat_no.Read())
								{
									if (cmd_inq_mat_no.GetString(1).Trim() != "")
									{
										if (v_strand_no == "C" || v_strand_no == "E")
										{
											v_mat_no = v_mat_no + "01";
										}
										else if (v_strand_no == "D" ||  v_strand_no == "F")
										{
											v_mat_no = v_mat_no + "02";
										}
										
									}
								}
								cmd_inq_mat_no.Close();
							}
						}
						cmd_inq.Close();
					}
					//材料号产出时为10位，分切后为12位。
					v_mat_no = v_mat_no;
				}
			}
			else
			{
				old_mat_no = bcls_rec->Tables[0].Rows[i]["MAT_NO"].ToString().Trim();
				v_mat_no = old_mat_no.SubstringNE(0, 10);

				//需要区分 分切和苹果切   35  分切
				if (bcls_rec->Tables[0].Rows[0]["DIV_FLAG"].ToString().Trim() == "35")
				{
					//获取材料号第十一位最大数据，并+1后赋给mat_no
					cmd_inq.SetCommandText("select NVL(MAX(SUBSTR(MAT_NO, LENGTH(MAT_NO) - 1 , 1 )),0) from tmmsm01 where mat_no like '" + old_mat_no.SubstringNE(0, 10) + "%'");
					cmd_inq.ExecuteReader();
					if (cmd_inq.Read())
					{
						if (cmd_inq.GetString(1).Trim() != "")
						{
							if ((CDecimal::Parse(cmd_inq.GetString(1).Trim()) + i + 1).ToString().Trim().GetLength() == 1)
							{
								Log::Trace("", __FUNCTION__, "35_mat_no	= [{0}]", (CDecimal::Parse(cmd_inq.GetString(1).Trim()) + i + 1).ToString().Trim());
								v_mat_no = v_mat_no + (CDecimal::Parse(cmd_inq.GetString(1).Trim()) + i + 1).ToString().Trim();
							}
							else
							{
								sprintf(s.msg, "该坯号切割超过10支，请重新确认！。");
								//strcpy(s.sysmsg,s.msg);
								throw CApplicationException(-1, s.msg, log.Location);
							}
						}
					}
					cmd_inq.Close();

					v_mat_no = v_mat_no + "0";
				}
				else if (bcls_rec->Tables[0].Rows[0]["DIV_FLAG"].ToString().Trim() == "36")
				{

					CString old_mat_11 = old_mat_no.SubstringNE(10, 1);//进来材料号的第11位
					//获取材料号第十一位最大数据，根据获取的值进行判断，当小于A时，则为35支之前的数据，则从数据库获取材料号前11位，最后一位为1-9  A-Z
					//如果大于A或等于A,则表示支数为32支之后，则第十一位为A-Z，第12位不变
					cmd_inq.SetCommandText("select NVL(MAX(SUBSTR(MAT_NO, LENGTH(MAT_NO) - 1 , 1 )),0),NVL(MAX(SUBSTR(MAT_NO, LENGTH(MAT_NO), 1 )),0) from tmmsm01 where mat_no like '" + old_mat_no.SubstringNE(0, 10) + "%'");
					cmd_inq.ExecuteReader();
					if (cmd_inq.Read())
					{
						mat_11 = cmd_inq.GetString(1).Trim()[0];
						mat_12 = cmd_inq.GetString(2).Trim()[0];
						//从数据库获取的第十一位大于等于A表示 产出指数为 32支后  
						//或是批量新增时 前一个材料号第十一位大于等于A时
						if (mat_11 >= 'A' || mat_new_11 >= 'A')
						{
							mat_new_11 = mat_11 + i + 1;
							if (mat_new_11 == 'I' || mat_new_11 == 'O')//|| mat_new_11 == 'Z'
							{
								mat_new_11 = mat_new_11 + 1;//当为这三个的时候跳过
							}
							if (mat_new_11 == 'Z')
							{
								sprintf(s.msg, "该坯号切割支数超过上限，请重新确认！。");
								throw CApplicationException(-1, s.msg, log.Location);
							}
							v_mat_no = v_mat_no + to_string(mat_new_11) + old_mat_11;
							Log::Trace("", __FUNCTION__, "mat_11= [{0}] mat_12 [{1}] v_mat_no [{2}]", (mat_11, mat_12, v_mat_no));
						}
						else if ((mat_12 + i + 1) == 'Z' && old_mat_11 == mat_11)
						{
							v_mat_no = v_mat_no + "A" + old_mat_11;//将原材料号11位的值放在最后一位上，11位的值从A开始
							mat_new_11 = 'A';//全局变量，当批量新增时，根据这个来判断是否已超过32支
						}
						else
						{
							if (mat_12 < 9 || mat_12 >= 'A')
							{
								mat_new_12 = mat_new_12 + i + 1;
								if (mat_new_12 == 'I' || mat_new_12 == 'O')//当为这两个的时候跳过
								{
									mat_new_12 = mat_new_12 + 1;
								}
								v_mat_no = v_mat_no + old_mat_11 + to_string(mat_new_12);
							}
							else if (mat_12 == 9)
							{
								v_mat_no = v_mat_no + old_mat_11 + "A";
							}


						}
					}
					cmd_inq.Close();

				}
			}


			Log::Trace("", __FUNCTION__, "v_mat_no	= [{0}]", v_mat_no);
			bcls_ret->Tables[0].Rows[i]["MAT_NO"] = v_mat_no;
			bcls_rec->Tables[0].Rows[i]["MAT_NO"] = v_mat_no;

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
	if (doFlag < 0)
	{
		//Log::Trace("", __FUNCTION__, "******************输出传入数据开始**********************");
		//tmmsm33.Print();
		//Log::Trace("", __FUNCTION__, "******************输出传入数据结束**********************");
	}
	return doFlag;

}
