/*********  MFJ  20240313  **********************/
/*********过钢量，回炉钢更改制造命令号**********************/
/*********改一整炉，传入参数老制造命令号和新制造命令号**********************/
/*********  **********************/
#include "stdafx.h"
#include "epex.h"



int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

BM2_FUNCTION_EXPORT


int f_mmsmggl_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 静态变量定义 ***** */
	int 	doFlag = 0;
	int 	ret = 0;
	int 	fetchRowCount = 0;
	CString	lpsz_tc_no = "";
	int   blkNum = 0;
	CString v_time = "";
	int  v_count1 = 0;
	int  v_count2 = 0;
	int sqlid = 0;
	CString v_old_pono = "";//老制造命令号
	CString v_pono = "";//新制造命令号
	CDecimal v_len = 0;//获取的长
	CDecimal v_width = 0;//获取的宽
	CDecimal v_thick = 0;//获取的厚
	CString v_factory_next = "";//计划去向  6391等
	CDecimal if_pipei = 0;//匹配成功与否   0  失败   1 成功
	CString  v_ingot_code = "";//锭型代码
	CDecimal v_slab_width = 0;//宽度  用来与称重系数表里的宽度进行比对
	CString v_cast_lot_no = "";//
	CDecimal v_lslab_no_length = 0;//长坯长度   为 0 则表示该命令坯为短坯
	CString v_slab_dest = "";//去向  10-1549热轧，11-2250热轧，20-型材，30-不锈线材，40-不锈热轧，41-4300厚板，50-外卖，60-二钢南区
	int slab_no_count = 0;//根据二级虚拟板坯号查找长批号对应的板坯号循环数
	CString sqlstr = "";
	CString sqlstr1 = "";
	CString sqlstr_ponoslab = "";
	int count_99 = 0;

	CModel tmmsm01("TMMSM01");
	CModel tmmsm33("TMMSM33");
	CModel tpssm10("TPSSM10");
	CModel tpssm40("TPSSM40");
	CModel tmmsm33bpgg("TMMSM33BPGG");//板坯规格上下限表
	CModel tmmsm96("TMMSM96");
	CModel tpssm03("TPSSM03");
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);
	CDbCommand cmd_inq_ponoslab(conn);
	int TotalRecordCount = 0;

	EIClass bcls_rec_MM04;//材料质量封锁
	bcls_rec_MM04.Tables[0].set_TableName("MM0099");
	bcls_rec_MM04.Tables[0].Columns.Add(tmmsm96);
	EIClass bcls_rec_MM2N;//材料质量封锁
	bcls_rec_MM2N.Tables[0].set_TableName("MM0099");
	bcls_rec_MM2N.Tables[0].Columns.Add(tmmsm96); 
	try
	{

		v_old_pono = bcls_rec->Tables[0].Rows[0]["OLD_PONO"].ToString().Trim();//老PONO
		v_pono = bcls_rec->Tables[0].Rows[0]["PONO"].ToString().Trim();//新PONO

		//传入参数不能为空
		if (v_old_pono == "" || v_pono == "")
		{
			strcpy(s.sysmsg, "传入的老制造命令号和新制造命令号不可为空!");
			strcpy(s.msg, s.sysmsg);
			throw CApplicationException(-1, s.msg, log.Location);
		}


		EIClass bcls_rec_TMSM01;
		bcls_rec_TMSM01.Tables[0].set_TableName("TMMSM01");
		bcls_rec_TMSM01.Tables[0].Columns.Add(tmmsm01);
		sqlstr1 = "SELECT COUNT(1) FROM （SELECT *  FROM  TMMSM01 WHERE PONO = '" + v_old_pono + "' AND RCV_MAT_FLAG = 'S'  UNION ALL SELECT *  FROM HMMSM01 WHERE PONO = '" + v_old_pono + "')  ";
		cmd_inq1.SetCommandText(sqlstr1);
		TotalRecordCount = cmd_inq1.ExecuteScalar().ToInt32();;
		cmd_inq1.Close();
		if (TotalRecordCount > 0)
		{
			CFormattable arguments[] = { v_old_pono }; // 定义参数列表的数组
			CMessageFormat::Format(s.msg, "制造命令[{0}],该炉已有产出，且产出有材料信息已收货。", arguments, 12); //格式化字符串
			throw CApplicationException(-1, s.msg, log.Location);
		}
		sqlstr1 = "SELECT COUNT(1) FROM （SELECT *  FROM  TMMSM01 WHERE PONO = '" + v_pono + "' AND RCV_MAT_FLAG = 'S'  UNION ALL SELECT *  FROM HMMSM01 WHERE PONO = '" + v_pono + "')  ";
		cmd_inq1.SetCommandText(sqlstr1);
		TotalRecordCount = cmd_inq1.ExecuteScalar().ToInt32();;
		cmd_inq1.Close();
		if (TotalRecordCount > 0)
		{
			CFormattable arguments[] = { v_pono }; // 定义参数列表的数组
			CMessageFormat::Format(s.msg, "制造命令[{0}],该炉已有产出，且产出有材料信息已收货。", arguments, 12); //格式化字符串
			throw CApplicationException(-1, s.msg, log.Location);
		}


		sqlstr = "SELECT * FROM TMMSM01 WHERE PONO = '" + v_old_pono + "'";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_rec_TMSM01.Tables[0]);
		cmd_inq.Close();

		for (int i = 0; i < bcls_rec_TMSM01.Tables[0].Rows.get_Count(); i++)
		{

			tmmsm01.Reset();
			tmmsm01.MergeFrom(bcls_rec_TMSM01.Tables[0].Rows[i]);

			tmmsm01["PONO"] = v_pono;//01表PONO取新PONO

			//根据新PONO查询计划表
			tpssm10["PONO"] = v_pono;
			if (!tpssm10.Query("PONO"))//若10表没有，则查询40表
			{
				tpssm40["PONO"] = v_pono;
				tpssm40.Query("PONO");
			}


			// 将合同信息去除
			tmmsm01["LSLAB_NO"] = " ";
			tmmsm01["PONO_SLAB"] = " ";         /*命令板坯号*/
			tmmsm01["PONO_SLAB_1"] = " ";           /*命令板坯号*/
			tmmsm01["PONO_SLAB_2"] = " ";           /*命令板坯号*/
			tmmsm01["PONO_SLAB_3"] = " ";           /*命令板坯号*/
			tmmsm01["PONO_SLAB_4"] = " ";           /*命令板坯号*/
			tmmsm01["PONO_SLAB_5"] = " ";           /*命令板坯号*/
			tmmsm01["PONO_SLAB_6"] = " ";           /*命令板坯号*/
			tmmsm01["PONO_SLAB_7"] = " ";           /*命令板坯号*/
			tmmsm01["PONO_SLAB_8"] = " ";           /*命令板坯号*/
			tmmsm01["PONO_SLAB_9"] = " ";           /*命令板坯号*/
			tmmsm01["PONO_SLAB_10"] = " ";           /*命令板坯号*/
			tmmsm01["PONO_SLAB_11"] = " ";           /*命令板坯号*/
			tmmsm01["PONO_SLAB_12"] = " ";           /*命令板坯号*/
			tmmsm01["FIN_ST_NO"] = " ";           /*命令板坯号*/
			tmmsm01["ORDER_NO"] = " ";           /*命令板坯号*/
			tmmsm01["PREC_SLAB_NO"] = " ";
			tmmsm96.Reset();
			tmmsm96.CopyFrom(tmmsm01);
			tmmsm96["EVENT_ID"] = "MM04";
			tmmsm96["EVENT_LINE_TYPE"] = "SM";
			tmmsm96["SYSTEM_ID"] = "MMSM";
			tmmsm96["FUNC_ID"] = "f_mmsmggl_proc";

			Log::Info("", __FUNCTION__, "cast_lot_no000       = [{0}]", tmmsm96["MAT_NO"].ToString());
			bcls_rec_MM04.Tables["MM0099"].Rows.Add();
			bcls_rec_MM04.Tables["MM0099"].Rows[i].Merge(tmmsm96);
			

			tmmsm96.CopyFrom(tmmsm01);
			tmmsm96["EVENT_ID"] = "MM2N";
			tmmsm96["EVENT_LINE_TYPE"] = "SM";
			tmmsm96["SYSTEM_ID"] = "MMSM";
			tmmsm96["FUNC_ID"] = "f_mmsmggl_proc";
			Log::Info("", __FUNCTION__, "cast_lot_no111       = [{0}]", tmmsm96["MAT_NO"].ToString());
			bcls_rec_MM2N.Tables["MM0099"].Rows.Add();
			bcls_rec_MM2N.Tables["MM0099"].Rows[i].Merge(tmmsm96);

			

		}

		doFlag = f_mmsm99(&bcls_rec_MM04, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		doFlag = f_mmsm99(&bcls_rec_MM2N, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}







	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "数据库处理出错，sqlcode=[{0}],请联系开发人员", arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}


