/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     王建征
Version:    1.0
Date:       2020-07-14
Description: 炼钢期初数据导入
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 炼钢期初数据导入
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
//#include "tmmsmbg.h"  



//外部函数声明
BM2_FUNCTION_EXPORT
int f_mmsm99(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);
BM2_FUNCTION_EXPORT
int f_mm0013(CDecimal  w_weight,		/* Weight 		    (t)		*/
CDecimal  w_width,						/* Width		    (mm)    */
CDecimal  w_thick,						/* Thickness	    (mm)	*/
CDecimal  w_density,					/* Material density (g/cm3) */
CDecimal  w_ctwg,						/* Coating weight   (g/m2)  */
CDecimal&  w_length,					/* Length		    (m)     */
CDbConnection * conn);
int f_mmsm_get_theorywt(CDecimal MAT_ACT_THICK, CDecimal MAT_ACT_WIDTH, CDecimal MAT_ACT_LEN, CDecimal MAT_NUM, CDecimal &MAT_THEORY_WT, CDbConnection * conn);//获取理重
int f_mm0011(CString SeqName, CDecimal SeqLen, CString &SeqNo, CDbConnection * conn);	//获取流水号



BM2F_ENTERACE(mmsmbg_import)

int f_mmsmbg_import(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{

	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString	datetime("");
	CDecimal w_density = 7.85;
	CDecimal w_ctwg = 0;
	CString	cs_mm00_mat_track_no("");//材料跟踪号的后4位流水号
	CString whole_backlog_code_temp("");//实际工序_临时
	CString cs_seq_no = "";
	CString cs_mm_id_seq_no = "";
	CString cs_mm_resume_seq_no = "";
	CDecimal mat_wt_t = 0;//理论重量计算单支重
	CDecimal mat_wt_a = 0;//实际重量计算单支重

	/* 实体类定义 */
	//CTMMSMBG tmmsmbg(conn);
	CModel tmmsm01qc("TMMSM01QC");
	CModel tmmsm01("TMMSM01");
	CModel tmmsm96("TMMSM96");
	CModel hmmsm01("HMMSM01");
	CModel hmmsm96("HMMSM96");
	CModel tmmsm33("TMMSM33");
	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_mat_inq(conn);

	EIClass mmsm_01;

	//期初数据DBLink名:DBLINK_HBC1，用户HBC1

	/* 添加并设置块名 */
	blkNum = bcls_rec->Tables.IndexOf("MM0099");
	if (blkNum < 0)
	{
		bcls_rec->Tables.Add("MM0099");
	}

	/*生成物料编码函数用*/
	blkNum = bcls_rec->Tables.IndexOf("MM0099_MAT_NO");
	if (blkNum < 0)
	{
		bcls_rec->Tables.Add("MM0099_MAT_NO");
		bcls_rec->Tables["MM0099_MAT_NO"].Columns.Add(DT_STRING, "MAT_SHAPE_FLAG");//材料形态
		bcls_rec->Tables["MM0099_MAT_NO"].Columns.Add(DT_STRING, "SG_SIGN");//牌号
		bcls_rec->Tables["MM0099_MAT_NO"].Columns.Add(DT_STRING, "SG_STD");//标准
		bcls_rec->Tables["MM0099_MAT_NO"].Columns.Add(DT_STRING, "PSC");//冶金规范码
		bcls_rec->Tables["MM0099_MAT_NO"].Columns.Add(DT_STRING, "WHOLE_BACKLOG_CODE");//工序码
		bcls_rec->Tables["MM0099_MAT_NO"].Columns.Add(DT_DECIMAL, "MAT_THICK");//厚度
		bcls_rec->Tables["MM0099_MAT_NO"].Columns.Add(DT_DECIMAL, "MAT_WIDTH");//宽度
	}
	bcls_rec->Tables["MM0099_MAT_NO"].Rows.Clear();

	try
	{
		//获取系统当前时刻
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		EIClass bcls_rec_MMBMZL;
		EIClass bcls_rec_WM02;


		sqlstr = " SELECT CODE,CODE_DESC_1_CONTENT FROM TWMSMZD02 WHERE  CODE_CLASS ='MMBMZL' ";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_rec_MMBMZL.Tables[0]);
		cmd_inq.Close();

		sqlstr = " select CODE,CODE_DESC_1_CONTENT from TWMSMZD02 where CODE_CLASS='WM02' ";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_rec_WM02.Tables[0]);
		cmd_inq.Close();


		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			//重置结构体待用
			tmmsm01qc.Reset();
			tmmsm01.Reset();
			tmmsm96.Reset();
			hmmsm01.Reset();
			hmmsm96.Reset();
			tmmsm33.Reset();

			if (bcls_rec->Tables[0].Rows[i]["HIS_FLAG"].ToString().Trim() == "TMMSM33")
			{
				tmmsm33.Reset();
				tmmsm33.MergeFrom(bcls_rec->Tables[0].Rows[i]);
				Log::Info("", __FUNCTION__, "MAT_NO=[{0}]", tmmsm33["MAT_NO"].ToString());
				if (tmmsm33.QueryCount("MAT_NO") > 0)
				{
					continue;
				}

				tmmsm33["REC_CREATOR"] = "TMMSM01QC";
				tmmsm33["REC_CREATE_TIME"] = datetime;
				tmmsm33.TrimOrBlank();
				Log::Info("", __FUNCTION__, "line=[{0}]", __LINE__);
				if (!tmmsm33.Insert())
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}
			else
			{



				//获取输入参数
				tmmsm01qc.MergeFrom(bcls_rec->Tables[0].Rows[i]);
				tmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[i]);

				//若该坯子已经有了，则跳过  暂定    mfj  20240319
				if (tmmsm01.QueryCount("MAT_NO") > 0)
				{
					continue;
				}

				for (int j = 0; j < bcls_rec_MMBMZL.Tables[0].Rows.get_Count(); j++)
				{
					if (tmmsm01qc["SURF_QUALITY"].ToString().Trim().Find("调宽") >= 0)
					{
						tmmsm01qc["ADJUST_WIDTH_MARK"] = "1";
					}
					if (tmmsm01qc["SURF_QUALITY"].ToString().Trim() == bcls_rec_MMBMZL.Tables[0].Rows[j]["CODE_DESC_1_CONTENT"].ToString().Trim())
					{
						tmmsm01qc["SURF_QUALITY"] = bcls_rec_MMBMZL.Tables[0].Rows[j]["CODE"].ToString().Trim();
					}
				}
				int if_fuhe = 0;
				for (int y = 0; y < bcls_rec_WM02.Tables[0].Rows.get_Count(); y++)
				{
					
					if (tmmsm01qc["GUIDE_DEST"].ToString().Trim() == bcls_rec_WM02.Tables[0].Rows[y]["CODE_DESC_1_CONTENT"].ToString().Trim())
					{
						tmmsm01qc["GUIDE_DEST"] = bcls_rec_WM02.Tables[0].Rows[y]["CODE"].ToString().Trim();
				
						if_fuhe++;
					}
					
				
				}
				if (if_fuhe == 0)
				{
					tmmsm01qc["GUIDE_DEST"] = " ";
					tmmsm01qc["SPARE_ITEM_6"] = tmmsm01qc["SPARE_ITEM_6"].ToString() + "指导去向不存在";//lz-备注校验信息
				}
				tmmsm01qc["MAT_LINE_TYPE"] = "SM";
				tmmsm01qc["REC_CREATOR"] = "TMMSM01QC";
				tmmsm01qc["REC_CREATE_TIME"] = datetime;

				hmmsm01.CopyFrom(tmmsm01qc);
				if (hmmsm01.QueryCount("MAT_NO") > 0)
				{
					continue;
				}

				doFlag = f_mm0011("MM00_MAT_ID", 4, cs_mm_id_seq_no, conn);
				if (doFlag < 0 || cs_mm_id_seq_no.Trim() == "")
				{
					sprintf(s.msg, "获取 生产流水号 失败，请查看数据库sequence【MM00_MAT_ID】是否正常!");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				tmmsm01qc["MAT_ID"] = datetime + cs_mm_id_seq_no;

				///* 设置调用物料跟踪参数值 */
				//bcls_rec->Tables["MM0099"].Rows.Clear();
				//
				tmmsm01.CopyFrom(tmmsm01qc);
				tmmsm96.CopyFrom(tmmsm01qc);
				tmmsm96["REC_CREATOR"] = "TMMSM01QC";
				tmmsm96["REC_CREATE_TIME"] = datetime;
				tmmsm96["EVENT_ID"] = "MMQC";
				tmmsm96["EVENT_LINE_TYPE"] = "SM";
				tmmsm96["SYSTEM_ID"] = "MMSM";
				tmmsm96["FUNC_ID"] = "mmsmbg_import";
				tmmsm96["MAT_KIND"] = "SM";
				tmmsm96.MergeTo(bcls_rec->Tables["MM0099"], false);

				///* 调用物料跟踪函数 */
				//doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
				//if (doFlag < 0)
				//{
				//	throw CApplicationException(-1, s.msg, s.svc_name);
				//}


				doFlag = f_mm0011("MM00_RESUME_SEQ_NO", 6, cs_mm_resume_seq_no, conn);
				if (doFlag < 0 || cs_mm_resume_seq_no.Trim() == "")
				{
					sprintf(s.msg, "获取 生产流水号 失败，请查看数据库sequence【MM00_RESUME_SEQ_NO】是否正常!");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				tmmsm96["RESUME_SEQ_NO"] = datetime + cs_mm_resume_seq_no;

				if (bcls_rec->Tables[0].Rows[i]["HIS_FLAG"].ToString() == "T")
				{
					tmmsm01.TrimOrBlank();
					tmmsm96.TrimOrBlank();
					Log::Info("", __FUNCTION__, "line=[{0}]", __LINE__);
					//tmmsm01.Print();
					!tmmsm01.Insert();
					Log::Info("", __FUNCTION__, "line=[{0}]", __LINE__);
					!tmmsm96.Insert();
					tmmsm01qc["ARCHIVE_FLAG"] = "T";
				}
				else if (bcls_rec->Tables[0].Rows[i]["HIS_FLAG"].ToString() == "H")
				{
					tmmsm01qc["TRAN_END_TIME"] = tmmsm01qc["TRAN_TIME"];
					tmmsm01qc["ARCHIVE_TIME"] = tmmsm01qc["TRAN_TIME"];
					hmmsm01.CopyFrom(tmmsm01qc);
					hmmsm96.CopyFrom(tmmsm96);

					if (hmmsm01.QueryCount("MAT_NO") > 0)
					{
						continue;
					}
					
					hmmsm01.TrimOrBlank();
					hmmsm96.TrimOrBlank();
					Log::Info("", __FUNCTION__, "line=[{0}]", __LINE__);
					!hmmsm01.Insert();
					Log::Info("", __FUNCTION__, "line=[{0}]", __LINE__);
					!hmmsm96.Insert();
					

					tmmsm01qc["ARCHIVE_FLAG"] = "H";
				}





				if (tmmsm01qc.QueryCount("MAT_NO") > 0)
				{
					tmmsm01qc.Update("*", "MAT_NO");
				}
				else
				{
					tmmsm01qc.TrimOrBlank();
					Log::Info("", __FUNCTION__, "line=[{0}]", __LINE__);
					//tmmsm01qc.Print();
					tmmsm01qc.Insert();
				}

				Log::Trace("", "", "HEAT_NO = [{0}]", tmmsm01qc["HEAT_NO"].ToString());
			}

		}

		//非切断实绩数据时才走进来
		/*if (bcls_rec->Tables[0].Rows[0]["HIS_FLAG"].ToString() != "TMMSM33")
		{
			sqlstr = " BEGIN P_TMMSM01_INSTOCK();END; ";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();
		}*/

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
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
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;

}

