/*******************************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-07-04
Description: 板坯挂命令板坯
remark:本函数目前暂不考虑按批管理
***********************************************************************/
/***** C/C++ 的标准头文件部分 *****/ 
// New Include

//框架公用头文件，勿删
#include "stdafx.h"
/***** C++ 的业务头文件部分 *****/ 





int f_mmsm33_pickslab(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_pssm03_prodflag_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

//外部函数声明
BM2_FUNCTION_EXPORT
 int f_mmsm33_check_ponoslab(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
		/****** 定义函数名称 ***** */
	CString FunctionEname = "f_mmsm33_check_ponoslab";                //定义函数英文名称  
	CString FunctionCname = "板坯切断_信息新增";              //定义函数中文名称
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义
 
	/****** 自定义变量 ***** */
	int doFlag = 0;
	int ret = 0;
	int blkNum = 0;

	CString c_datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");          //当前时间
	CString check_flag = "";
	
	int mat_seq  = 0;
	int fetchRowCount = 0;
	CString sqlstr = "";
	EIClass inBlock;          //材料主档信息处理用
	//EIClass outBlock;

	CModel tmmsm01("TMMSM01");
	CModel tpssm03("TPSSM03");
	CDbCommand cmd_inq(conn);
	CModel tmmsm33("TMMSM33");
	CModel tmmsm96("TMMSM96");

	try
	{
		blkNum = bcls_rec->Tables.IndexOf("TMMSM33");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("TMMSM33");
			bcls_rec->Tables["TMMSM33"].Columns.Add(tmmsm33);
		}
		bcls_rec->Tables["TMMSM33"].Rows.Clear();

		/*物料跟踪履历用，f_mmsm99函数用*/
		blkNum = inBlock.Tables.IndexOf("MM0099");
		if (blkNum < 0)
		{
			inBlock.Tables.Add("MM0099");
			inBlock.Tables["MM0099"].Columns.Add(tmmsm96);
		}
		/*置板坯命令产出标志*/
		blkNum = bcls_rec->Tables.IndexOf("PSSM03");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("PSSM03");
			bcls_rec->Tables["PSSM03"].Columns.Add(DT_STRING, "FACTORY_DIV");
			bcls_rec->Tables["PSSM03"].Columns.Add(DT_STRING, "SLAB_NO");       //预定板坯号
			bcls_rec->Tables["PSSM03"].Columns.Add(DT_DECIMAL, "SLAB_NUM");       //预定板坯号
			bcls_rec->Tables["PSSM03"].Columns.Add(DT_STRING, "SLAB_PROD_FLAG");//产出标记	
		}
		bcls_rec->Tables["PSSM03"].Rows.Clear();
		
		if (bcls_rec->Tables[0].Columns.Contains("CHECK_FLAG"))
		{
			check_flag = bcls_rec->Tables[0].Rows[0]["CHECK_FLAG"];
		}
		else
		{
			check_flag = "9";
		}
		//若前台未配置板坯块数 则置默认1
		if (!bcls_rec->Tables[0].Columns.Contains("QTY"))
		{
			bcls_rec->Tables[0].Columns.Add(DT_DECIMAL, "QTY");
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				bcls_rec->Tables[0].Rows[i]["QTY"] = 1;
			}
		}
		

		if (check_flag.Trim() == "1" || check_flag.Trim() == "9")//核对
		{
			//命令板坯匹配类型  ""-不匹配 "L" 按长尺坯匹配  "S" 短尺坯匹配 
			if (!bcls_rec->Tables[0].Columns.Contains("PICK_TYPE"))
			{
				bcls_rec->Tables[0].Columns.Add(DT_STRING, "PICK_TYPE");
			}
			bcls_rec->Tables[0].Rows[0]["PICK_TYPE"] = "L";
			if (!bcls_rec->Tables[0].Columns.Contains("GETSLAB_MODE"))
			{
				bcls_rec->Tables[0].Columns.Add(DT_STRING, "GETSLAB_MODE");
			}
			if (bcls_rec->Tables[1].Rows.get_Count() <= 0)
			{
				bcls_rec->Tables[0].Rows[0]["GETSLAB_MODE"] = "A";
			}
			if (!bcls_rec->Tables[0].Columns.Contains("SL_FLAG"))
			{
				bcls_rec->Tables[0].Columns.Add(DT_STRING, "SL_FLAG");
			}


			if (!bcls_rec->Tables[0].Columns.Contains("SLAB_THICK"))
			{
				bcls_rec->Tables[0].Columns.Add(DT_STRING, "SLAB_THICK");
			}

			if (!bcls_rec->Tables[0].Columns.Contains("SLAB_WIDTH"))
			{
				bcls_rec->Tables[0].Columns.Add(DT_STRING, "SLAB_WIDTH");
			}

			if (!bcls_rec->Tables[0].Columns.Contains("SLAB_LEN"))
			{
				bcls_rec->Tables[0].Columns.Add(DT_STRING, "SLAB_LEN");
			}
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				bcls_rec->Tables[0].Rows[i]["SLAB_THICK"] = bcls_rec->Tables[0].Rows[i]["MAT_ACT_THICK"];
				bcls_rec->Tables[0].Rows[i]["SLAB_WIDTH"] = bcls_rec->Tables[0].Rows[i]["MAT_ACT_WIDTH"];
				bcls_rec->Tables[0].Rows[i]["SLAB_LEN"] = bcls_rec->Tables[0].Rows[i]["MAT_ACT_LEN"];
				bcls_rec->Tables["TMMSM33"].Rows.Add();
				bcls_rec->Tables["TMMSM33"].Rows[i].Merge(bcls_rec->Tables[0].Rows[i]);
				bcls_rec->Tables["TMMSM33"].Rows[i]["MAT_TUBE"] = 1;
			}

			bcls_rec->Tables[0].Rows[0]["SL_FLAG"] = "1";
			ret = f_mmsm33_pickslab(bcls_rec, bcls_ret, conn);
			if (ret < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}

			for (int i = 0; i < bcls_rec->Tables["TMMSM33"].Rows.get_Count(); i++)
			{
				bcls_rec->Tables[0].Rows[i].Merge(bcls_rec->Tables["TMMSM33"].Rows[i]);
				bcls_rec->Tables[0].Rows[i]["PONO_SLAB"] = bcls_rec->Tables[0].Rows[i]["PONO_SLAB_1"];//PONO_SLAB=PONO_SLAB_1
			}
			
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				tmmsm01.Reset();
				tmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[i]);
				/* 调物料跟踪 */
				inBlock.Tables["MM0099"].Rows.Clear();
				inBlock.Tables["MM0099"].Rows.Add(); // 创建一行
				inBlock.Tables["MM0099"].Rows[0].Merge(tmmsm01);
				inBlock.Tables["MM0099"].Rows[0]["EVENT_ID"] = "MM0S";
				inBlock.Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"] = "00";
				inBlock.Tables["MM0099"].Rows[0]["SYSTEM_ID"] = "MMSM";
				inBlock.Tables["MM0099"].Rows[0]["FUNC_ID"] = "f_mmsm33_check_ponoslab";
				inBlock.Tables["MM0099"].Rows[0]["EVENT_DESC"] = "板坯核对";
				CString c_slab_no_colmn = " ";
				CString c_slab_len_colmn = " ";
				CDecimal idx = 0;
				int v_fix_slab_num = 0;
				for (idx = 1; idx <= 8; idx = idx + 1)
				{
					c_slab_no_colmn = "PONO_SLAB_" + idx.ToString();
					if (inBlock.Tables["MM0099"].Rows[0][c_slab_no_colmn].ToString().Trim() > "")
					{
						v_fix_slab_num++;
						
						//取得命令铸坯号，并判断是否为空，如空跳过。
						CString ponoSlab = inBlock.Tables["MM0099"].Rows[0][c_slab_no_colmn].ToString().Trim();

						//Log::Trace("", __FUNCTION__, "ORD_MATtmmsm33["MAT_NO"] =[{0}]", tmmsm01["MAT_NO"].ToString());
						//Log::Trace("", __FUNCTION__, "ORD_MATponoSlab=[{0}]", ponoSlab);
					}
				}
				inBlock.Tables["MM0099"].Rows[0]["FIX_SLAB_NUM"] = v_fix_slab_num;
				doFlag = f_mmsm99(&inBlock, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}
			
		}
		
		if (check_flag.Trim() == "2" )//强制核对   单记录处理
		{
			bcls_rec->Tables["PSSM03"].Rows.Clear();
			tmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[0]);
			/* 调物料跟踪 */
			inBlock.Tables["MM0099"].Rows.Clear();
			inBlock.Tables["MM0099"].Rows.Add(); // 创建一行
			inBlock.Tables["MM0099"].Rows[0].Merge(tmmsm01);
			inBlock.Tables["MM0099"].Rows[0]["EVENT_ID"] = "MM0S";
			inBlock.Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"] = "00";
			inBlock.Tables["MM0099"].Rows[0]["SYSTEM_ID"] = "MMSM";
			inBlock.Tables["MM0099"].Rows[0]["FUNC_ID"] = "f_mmsm33_check_ponoslab";
			inBlock.Tables["MM0099"].Rows[0]["EVENT_DESC"] = "板坯强制核对";


			CString c_slab_no_colmn = " ";
			CDecimal idx = 1;
			int v_fix_slab_num = 0;
			
			for (int i = 0; i < bcls_rec->Tables[1].Rows.get_Count(); i++)
			{
				if (bcls_rec->Tables[1].Rows[i]["PONO_SLAB"].ToString()>" ")
				{
					c_slab_no_colmn = "PONO_SLAB_" + idx.ToString();
					inBlock.Tables["MM0099"].Rows[0][c_slab_no_colmn] = " ";
					idx = idx + 1;
					v_fix_slab_num++;
					//取得命令铸坯号，并判断是否为空，如空跳过。
					CString ponoSlab = bcls_rec->Tables[1].Rows[i]["PONO_SLAB"].ToString();
					CDataRow& row = bcls_rec->Tables["PSSM03"].Rows.Add();
					row["FACTORY_DIV"] = " ";
					row["SLAB_NO"] = ponoSlab;
					row["SLAB_NUM"] = 1;
					row["SLAB_PROD_FLAG"] = "1";  //0-未产出, 1-产出, 2-部分产出

					//Log::Trace("", __FUNCTION__, "ORD_MATtmmsm33["MAT_NO"] =[{0}]", tmmsm01["MAT_NO"].ToString());
					//Log::Trace("", __FUNCTION__, "ORD_MATponoSlab=[{0}]", ponoSlab);
				}
			}
			inBlock.Tables["MM0099"].Rows[0]["PONO_SLAB"] = inBlock.Tables["MM0099"].Rows[0]["PONO_SLAB_1"];
			inBlock.Tables["MM0099"].Rows[0]["FIX_SLAB_NUM"] = v_fix_slab_num;
			doFlag = f_mmsm99(&inBlock, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			if (bcls_rec->Tables["PSSM03"].Rows.get_Count()>0)
			{
				//命令铸坯置产出标记。其中要判断数量，达到需求数量，则置产出完成(1)。
				doFlag = f_pssm03_prodflag_upd(bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}

		}

		if (check_flag.Trim() == "0")//核对取消
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				CString c_slab_no_colmn = " ";
				CString c_slab_len_colmn = " ";
				CDecimal idx = 0;
				int v_fix_slab_num = 0;
				int pssm03_count = 0;
				for (int j = 1; j <= 12; j++)
				{
					if (bcls_rec->Tables[0].Rows[i]["PONO_SLAB_" + CConvert::ToString(j)].ToString().Trim() == "")
						break;
					bcls_rec->Tables["PSSM03"].Rows.Add();
					bcls_rec->Tables["PSSM03"].Rows[pssm03_count]["FACTORY_DIV"] = " ";
					bcls_rec->Tables["PSSM03"].Rows[pssm03_count]["SLAB_NO"] = bcls_rec->Tables[0].Rows[i]["PONO_SLAB_" + CConvert::ToString(j)];
					bcls_rec->Tables[0].Rows[i]["PONO_SLAB_" + CConvert::ToString(j)] = " ";//置为空值
					bcls_rec->Tables["PSSM03"].Rows[pssm03_count]["SLAB_NUM"] = 1;
					bcls_rec->Tables["PSSM03"].Rows[pssm03_count]["SLAB_PROD_FLAG"] = "0";
					pssm03_count++;
				}

				tmmsm01.Reset();
				tmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[i]);

				tmmsm01["PONO_SLAB"] = tmmsm01["PONO_SLAB_1"];
				tmmsm01["LSLAB_NO"] = " ";


				/* 调物料跟踪 */
				inBlock.Tables["MM0099"].Rows.Clear();
				inBlock.Tables["MM0099"].Rows.Add(); // 创建一行
				inBlock.Tables["MM0099"].Rows[0].Merge(tmmsm01);
				inBlock.Tables["MM0099"].Rows[0]["EVENT_ID"] = "MM0S";
				inBlock.Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"] = "00";
				inBlock.Tables["MM0099"].Rows[0]["SYSTEM_ID"] = "MMSM";
				inBlock.Tables["MM0099"].Rows[0]["FUNC_ID"] = "f_mmsm33_check_ponoslab";
				inBlock.Tables["MM0099"].Rows[0]["FIX_SLAB_NUM"] = "0";
				inBlock.Tables["MM0099"].Rows[0]["EVENT_DESC"] = "板坯核对取消";
				doFlag = f_mmsm99(&inBlock, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}


			}
			//Log::Trace("", "", "111111111111111111111111111={0}", bcls_rec->Tables["PSSM03"].Rows.get_Count());
			if (bcls_rec->Tables["PSSM03"].Rows.get_Count()>0)
			{
				doFlag = f_pssm03_prodflag_upd(bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			
		}
		
		
		
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	if (doFlag < 0)
	{
		//Log::Trace("", __FUNCTION__, "******************输出传入数据开始**********************");
		//tmmsm01.Print();
		//Log::Trace("", __FUNCTION__, "******************输出传入数据结束**********************");
	}
	return doFlag;

}
