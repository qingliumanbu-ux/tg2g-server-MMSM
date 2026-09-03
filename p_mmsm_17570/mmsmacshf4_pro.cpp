/************************/
/*** 2023-11-13 ********/
/****   mfj **************/
/**** 收货确认 ***********/
/************************/



//框架头文件
#include "stdafx.h"
//程序用头文件
//电文发送头文件
#include "epex.h"

int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsmacsh_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_wmsmsm_stock_in(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);//调用仓库接口，进行板坯入库 
int f_mm0011(CString SeqName, CDecimal SeqLen, CString &SeqNo, CDbConnection * conn);	//获取流水号
// service入口
BM2F_ENTERACE(mmsmacshf4_pro)
BM2_FUNCTION_EXPORT
int f_mmsmacshf4_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDecimal Count = 0;
	CString v_update = "";
	CString s_formname = "";
	CString i_func_id = "";
	CString i_func = "";
	CString v_fields_str = "";
	CDecimal v_celm_act = 0;//碳元素
	CString v_shll_seq = "";//收货履历序号
	int blkNum = 0;
	CString v_resume_seq_no = "";//序号
	int batch_flag = 0;//批量收货的数据条数 大于1表示为批量收货，此时收货报错就跳过   1 批量收货   0 单只收货

	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsm33("TMMSM33");
	CModel tmmsm33shll("TMMSM33SHLL");//收货履历表，原始记录，每次收货新增进表后 不再更改改该数据  主键:材料号，序号
	CModel tmmsm01("TMMSM01");
	CModel tmmsm01_query("TMMSM01");
	CModel tmmsm96("TMMSM96");
	CModel tmmsm3e("TMMSM3E");

	CDbCommand cmd_inq(conn);

	try
	{

		Log::Trace("", __FUNCTION__, "COUTNT[{0}]  ", bcls_rec->Tables[0].Rows.get_Count());

		blkNum = bcls_rec->Tables.IndexOf("MM0099");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MM0099");
			bcls_rec->Tables["MM0099"].Columns.Add(tmmsm96);
		}

		blkNum = bcls_rec->Tables.IndexOf("MMSMACSH");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MMSMACSH");
			bcls_rec->Tables["MMSMACSH"].Columns.Add(tmmsm96);
		}

		bcls_rec->Tables["MMSMACSH"].Columns.Add(DT_STRING, "DEAL_FLAG");


		/**  调用仓库接口  进行入库操作    mfj  2023.12.19  太钢定制 **/
		/*置板坯命令产出标志*/
		blkNum = bcls_rec->Tables.IndexOf("WM_STOCK");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("WM_STOCK");
			bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "MAT_NO");
			bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "STOCK_OPER_ORDER");        //库业务类型
			bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_DECIMAL, "STOCK_OPER_ORDER_DIV");   //业务类型内区分
			bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "STOCK_NO");				//库号
			bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "STOCK_PLACE_NO");			//材料库位号
			bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "ROWNO");					//行号
			bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "COLUMN_NO");				//列号
			bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "LAYERNO");					//层号
			bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "STOCK_PLACE_POSITION");	//库位内位置
		}

		//当为批量收货时，该标记为1   当批量收货时，有条件卡住时，不报错，跳过处理
		if (bcls_rec->Tables[0].Rows.get_Count() > 1)
		{
			batch_flag = 1;
		}


		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			//将结构体清空
			tmmsm01.Reset();
			tmmsm96.Reset();
			tmmsm01_query.Reset();
			tmmsm33shll.Reset();
			v_celm_act = 0;

			tmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			/*tmmsm01.Print();*/
			
			

			Log::Trace("", __FUNCTION__, "get_Count[{0}]  ", bcls_rec->Tables[0].Rows.get_Count());
			/*Log::Trace("", __FUNCTION__, "PARAget_Count[{0}]  ", bcls_rec->Tables["PARA"].Rows.get_Count());
			Log::Trace("", __FUNCTION__, "PRODUCT_FLAG[{0}]  ", bcls_rec->Tables["PARA"].Rows[0]["PRODUCT_FLAG"].ToString());
			Log::Trace("", __FUNCTION__, "RECEIVE_WEIGHT[{0}]  ", bcls_rec->Tables["PARA"].Rows[0]["RECEIVE_WEIGHT"].ToString());
			Log::Trace("", __FUNCTION__, "RECV_MAT_TIME[{0}]  ", bcls_rec->Tables["PARA"].Rows[0]["RECV_MAT_TIME"].ToString());*/
			
			

			tmmsm01_query.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			if (!tmmsm01.QueryCount("MAT_NO"))//TMMSM01表未获取数据
			{
				if (batch_flag == 1)
				{
					if (batch_flag == 1)
					{
						Log::Trace("", __FUNCTION__, "材料[{0}]在线档中不存在数据，请确认数据是否已归档！！", tmmsm01_query["MAT_NO"].ToString());
						continue;
					}
					continue;
				}
				Log::Trace("", __FUNCTION__, "MAT_NO[{0}]  ", tmmsm01["MAT_NO"].ToString());
				strcpy(s.sysmsg, "在线档中不存在数据，请确认数据是否已归档!");
				strcpy(s.msg, s.sysmsg);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			tmmsm01_query.Query("MAT_NO");

			//当传入的钢种与当前钢种不一致时 根据钢种查询钢牌号，将钢牌号更新掉
			//只允许在盘库画面去操作
			/*if (tmmsm01["ST_NO"].ToString() != tmmsm01_query["ST_NO"].ToString())
			{
				sqlstr = "SELECT  SG_GRADE_1  FROM TQMTS0X  WHERE ST_NO = '" + tmmsm01["ST_NO"].ToString().Trim() + "'";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					tmmsm01["SG_GRADE_1"] = cmd_inq.GetString(1);
				}

				tmmsm01["ORDER_NO"] = " ";
			}
			cmd_inq.Close();*/


			if (bcls_rec->Tables[0].Rows[i]["SLAB_TYPE_OLD"].ToString().Trim() == "8"&&
				tmmsm01_query["LSLAB_NO"].ToString().Trim() != "")
			{
				/*if (batch_flag == 1)
				{
					Log::Trace("", __FUNCTION__, "材料[{0}]当前计划已关闭，且未脱虚拟板坯，不允许收货！！", tmmsm01_query["MAT_NO"].ToString());
					continue;
				}*/
				strcpy(s.sysmsg, "当前计划已关闭，且未脱虚拟板坯，不允许收货!");
				strcpy(s.msg, s.sysmsg);
				throw CApplicationException(-1, s.msg, log.Location);
			}


			//因计划的物料编码存在获取不到的情况，故命令坯有值但成品标记为空的情况。故此判断暂不添加  mfj  20240507
			if (false)
			{

				//增加校验，只有余材时，才可以更改成品标记
					if (tmmsm01_query["PRODUCT_FLAG"].ToString().Trim() != tmmsm01["PRODUCT_FLAG"].ToString().Trim()
				&& tmmsm01_query["PONO_SLAB"].ToString().Trim() != "")
				{
				sprintf(s.sysmsg, "材料[%s]必须是余材时，才可更改成品标记！！", (const char*)tmmsm01_query["MAT_NO"].ToString());
				strcpy(s.msg, s.sysmsg);
				throw CApplicationException(-1, s.msg, log.Location);
				}

			}

			

			if (tmmsm01_query["RCV_MAT_FLAG"].ToString().Trim() == "S" || tmmsm01_query["RCV_MAT_FLAG"].ToString().Trim() == "W")
			{
				if (batch_flag == 1)
				{
					Log::Trace("", __FUNCTION__, "材料[{0}]已经处于收货状态，无法再次收货！！", tmmsm01_query["MAT_NO"].ToString());
					continue;
				}
				sprintf(s.sysmsg, "材料[" + tmmsm01_query["MAT_NO"].ToString() + "]已经处于收货状态，无法再次收货！！");
				strcpy(s.msg, s.sysmsg);
				throw CApplicationException(-1, s.msg, log.Location);
			}


			tmmsm33.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			//单判碳元素  手动收货不卡成分   自动收货卡成分
			//switch (conn->DatabaseKind)
			//{
			//case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			//case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			//case DB_KIND_MSSQL:				// MS SQL Server数据库
			//case DB_KIND_ORACLE:	        // Oracle 数据库
			//default:

			//	sqlstr = "select ELM_ACT FROM TQMTS29 WHERE HEAT_NO = '"+tmmsm01["HEAT_NO"].ToString().Trim()+"' AND ELM_NAME = 'C'";
			//}
			//cmd_inq.SetCommandText(sqlstr);
			//cmd_inq.ExecuteReader();
			//if (cmd_inq.Read())
			//{
			//	v_celm_act = cmd_inq.GetDecimal(1);
			//}
			//cmd_inq.Close();

			//Log::Trace("", __FUNCTION__, "v_celm_act[{0}]  ", v_celm_act);

			//若没有碳元素值，则不允许收货
		/*	if (v_celm_act == 0)
			{
				strcpy(s.sysmsg, "碳元素没有值，请确认后再进行收货!");
				strcpy(s.msg, s.sysmsg);
				throw CApplicationException(-1, s.msg, log.Location);
			}*/

			//收货重量不允许为0   若为0，则取系统重量  MFJ  20240222
			//改为取实时重量(若有称重量则为称重量，否则为三级理论量)  MFJ  20240309
			if (tmmsm01["RECEIVE_WEIGHT"].ToDecimal() > 100)
			{
				strcpy(s.sysmsg, "收获重量不允许大于100，请重新确认!");
				strcpy(s.msg, s.sysmsg);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (tmmsm01["RECEIVE_WEIGHT"].ToDecimal() == 0 || tmmsm01["RECEIVE_WEIGHT"].ToDecimal() < 0)
			{
				tmmsm01["RECEIVE_WEIGHT"] = tmmsm01_query["MEASURE_WT"];
				if (tmmsm01["RECEIVE_WEIGHT"].ToDecimal() == 0 || tmmsm01["RECEIVE_WEIGHT"].ToDecimal() < 0)
				{
					tmmsm01["RECEIVE_WEIGHT"] = tmmsm01_query["MAT_WT"];
				}

				/*strcpy(s.sysmsg, "收获重量不允许小于等于0，请重新确认!");
				strcpy(s.msg, s.sysmsg);
				throw CApplicationException(-1, s.msg, log.Location);*/
			}

			tmmsm01["MAT_ACT_WT"] = tmmsm01["RECEIVE_WEIGHT"];  //将实际重量复制，用来发电文时获取用户输入数据
			Log::Trace("", __FUNCTION__, "RECEIVE_WEIGHT[{0}]  ", tmmsm01["RECEIVE_WEIGHT"].ToDecimal());
			Log::Trace("", __FUNCTION__, "MAT_ACT_WT[{0}]  ", tmmsm01["MAT_ACT_WT"].ToDecimal());
			

			if (tmmsm01["RECV_MAT_TIME"].ToString().Trim() == "")
			{
				tmmsm01["RECV_MAT_TIME"] = datetime;
			}

			////批量处理时，弹窗中有数据，则取弹窗中数据，否则取表格中数据
			//if (bcls_rec->Tables["PARA"].Rows[0]["RECEIVE_WEIGHT"].ToDecimal() > 0)
			//{
			//	tmmsm01["RECEIVE_WEIGHT"] = bcls_rec->Tables["PARA"].Rows[0]["RECEIVE_WEIGHT"].ToDecimal();
			//}
			//if (bcls_rec->Tables["PARA"].Rows[0]["RECV_MAT_TIME"].ToString().Trim() != "")
			//{
			//	tmmsm01["RECV_MAT_TIME"] = bcls_rec->Tables["PARA"].Rows[0]["RECV_MAT_TIME"].ToDecimal();
			//}
			//if (bcls_rec->Tables["PARA"].Rows[0]["PRODUCT_FLAG"].ToString().Trim() != "")
			//{
			//	tmmsm01["PRODUCT_FLAG"] = bcls_rec->Tables["PARA"].Rows[0]["PRODUCT_FLAG"].ToString().Trim();
			//}

			
			
			//放在收货反馈里  mfj  20240424
			/*tmmsm33["RECEIVE_WEIGHT"] = tmmsm01["RECEIVE_WEIGHT"];
			tmmsm33["RCV_MAT_FLAG"] = "W";  //N 未收货  W等待(等L4的反馈)  S收货成功
			tmmsm33["RECV_MAT_TIME"] = tmmsm01["RECV_MAT_TIME"];
			tmmsm33.TrimOrBlank();
			tmmsm33.Update("RECEIVE_WEIGHT,RCV_MAT_FLAG,RECV_MAT_TIME","MAT_NO");*/


			doFlag = f_mm0011("TMMSM3E_seq", 8, v_resume_seq_no, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
			tmmsm3e.CopyFrom(tmmsm01);
			tmmsm3e["PROD_TIME"] = datetime;
			tmmsm3e["RECEIVE_BACK_STATUS"] = "W";
			tmmsm3e["REMARK"] = "收货等待反馈";
			tmmsm3e["RESUME_SEQ_NO"] = datetime + v_resume_seq_no;
			tmmsm3e.TrimOrBlank();
			tmmsm3e.Insert();


			//RECEIVE_WEIGHT,RCV_MAT_FLAG,RECV_MAT_TIME
			tmmsm01["SLAB_HEAD_WIDTH"] = tmmsm01["MAT_WIDTH"];
			tmmsm01["SLAB_TAIL_WIDTH"] = tmmsm01["MAT_WIDTH"];
			tmmsm01["MAT_ACT_WIDTH"] = tmmsm01["MAT_WIDTH"];
			tmmsm01["MAT_ACT_LEN"] = tmmsm01["MAT_LEN"];
			tmmsm01["MAT_ACT_THICK"] = tmmsm01["MAT_THICK"];

			//在反馈中处理   mfj  20240424
			/*tmmsm96.CopyFrom(tmmsm01);
			tmmsm96["MAT_NO"] = tmmsm01["MAT_NO"];
			//tmmsm96["RECEIVE_WEIGHT"] = tmmsm01["MAT_WT"];
			tmmsm96["RECEIVE_WEIGHT"] = tmmsm01["RECEIVE_WEIGHT"];//收货重量，同时更新名义和实际重量，实际重量即老系统的系统重量   mfj  20240113
			tmmsm96["MAT_WT"] = tmmsm01["RECEIVE_WEIGHT"];
			tmmsm96["MAT_ACT_WT"] = tmmsm01["RECEIVE_WEIGHT"];
			tmmsm96["QUALIFIED_WT"] = tmmsm01["RECEIVE_WEIGHT"];//合格产量
			//tmmsm96["REAL_TIME_WT"] = tmmsm01["MAT_WT"]; //实时重量 
			

			Log::Trace("", __FUNCTION__, "RECEIVE_WEIGHT[{0}]  ", tmmsm01["RECEIVE_WEIGHT"].ToString());

			//名义规格与实际规格保持一致
			tmmsm96["MAT_WIDTH"] = tmmsm01["MAT_WIDTH"];
			tmmsm96["MAT_LEN"] = tmmsm01["MAT_LEN"];
			tmmsm96["MAT_THICK"] = tmmsm01["MAT_THICK"];

			//画面修改名义，用名义覆盖实际 保持两套一致
			tmmsm96["MAT_ACT_WIDTH"] = tmmsm01["MAT_WIDTH"];
			tmmsm96["MAT_ACT_LEN"] = tmmsm01["MAT_LEN"];
			tmmsm96["MAT_ACT_THICK"] = tmmsm01["MAT_THICK"];
			tmmsm96["MAT_ACT_WT"] = tmmsm01["RECEIVE_WEIGHT"];
			tmmsm96["SLAB_HEAD_WIDTH"] = tmmsm01["MAT_WIDTH"];//头宽
			tmmsm96["SLAB_TAIL_WIDTH"] = tmmsm01["MAT_WIDTH"];//尾宽
			tmmsm96["SLAB_NO"] = tmmsm01["SLAB_NO"];
			//tmmsm96["SG_SIGN"] = tmmsm01["SG_SIGN"];  钢种修改在盘库画面 未收货时才可以修改 mfj  20240416
		
			//tmmsm96["ACCEP_STLOC"] = "6242";
			tmmsm96["RECV_MAT_TIME"] = tmmsm01["RECV_MAT_TIME"];//收货日期
			tmmsm96["PRODUCT_FLAG"] = tmmsm01["PRODUCT_FLAG"];//成品标记
			tmmsm96["RCV_MAT_FLAG"] = "W";//N 未收货  W等待(等L4的反馈)  S收货成功
			tmmsm96["RCV_NO_STATUS"] = "N"; // 收货取消状态  置为 未取消  mfj  20240309
			tmmsm96["EVENT_ID"] = "MM34";
			tmmsm96["EVENT_LINE_TYPE"] = "SM";
			tmmsm96["FUNC_ID"] = "mmsmacshf4_pro";
			tmmsm96["SYSTEM_ID"] = "MMSM";
			tmmsm96["EVENT_DESC"] = "铸坯收货确认"; */


			//将数据新增进收货履历表里
			tmmsm33shll.CopyFrom(tmmsm01);
			tmmsm33shll["MAT_WT"] = tmmsm01["RECEIVE_WEIGHT"];
			tmmsm33shll["MAT_ACT_WT"] = tmmsm01["RECEIVE_WEIGHT"];
			tmmsm33shll["QUALIFIED_WT"] = tmmsm01["RECEIVE_WEIGHT"];//合格产量
			tmmsm33shll["MAT_ACT_WIDTH"] = tmmsm01["MAT_WIDTH"];
			tmmsm33shll["MAT_ACT_LEN"] = tmmsm01["MAT_LEN"];
			tmmsm33shll["MAT_ACT_THICK"] = tmmsm01["MAT_THICK"];
			tmmsm33shll["RCV_MAT_FLAG"] = "W";//N 未收货  W等待(等L4的反馈)  S收货成功 
			

			doFlag = f_mm0011("TMMSM33SHLL_SEQ", 8, v_shll_seq, conn);
			if (doFlag < 0 || v_shll_seq.Trim() == "")
			{
				strcpy(s.msg, "物料材料跟踪号后8位流水号生成错误！");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}


			tmmsm33shll["REC_CREATE_TIME"] = datetime;
			tmmsm33shll["REC_CREATOR"] = s.userid;
				
				
			tmmsm33shll["RESUME_SEQ_NO"] = datetime + v_shll_seq;
			tmmsm33shll.TrimOrBlank();
			tmmsm33shll.Insert();

			tmmsm96.CopyFrom(tmmsm01);
			tmmsm96["EVENT_ID"] = "MM3H";
			tmmsm96["EVENT_LINE_TYPE"] = "SM";
			tmmsm96["SYSTEM_ID"] = "MMSM";
			tmmsm96["FUNC_ID"] = "mmsmacshf4_pro";
			tmmsm96["MAT_NO"] = tmmsm01["MAT_NO"];

			tmmsm96["RCV_MAT_FLAG"] = "W";
			tmmsm96["EVENT_DESC"] = "收货确认等待反馈";

			

			bcls_rec->Tables["MM0099"].Rows.Add();
			bcls_rec->Tables["MM0099"].Rows[i].Merge(tmmsm96);


			bcls_rec->Tables["MMSMACSH"].Rows.Add();
			bcls_rec->Tables["MMSMACSH"].Rows[i].Merge(tmmsm96);
			bcls_rec->Tables["MMSMACSH"].Rows[i]["DEAL_FLAG"] = "N";



			#pragma region 调用仓库接口，进行入库操作   太钢定制   后续要更改为，收到L4反馈收货成功后才调用入库
			if (true) //
			{
				bcls_rec->Tables["WM_STOCK"].Rows.Add();
				bcls_rec->Tables["WM_STOCK"].Rows[i]["MAT_NO"] = tmmsm01["MAT_NO"];
				bcls_rec->Tables["WM_STOCK"].Rows[i]["STOCK_OPER_ORDER"] = "1B";					//库业务类型
				bcls_rec->Tables["WM_STOCK"].Rows[i]["STOCK_OPER_ORDER_DIV"] = "1";					//业务类型内区分
				bcls_rec->Tables["WM_STOCK"].Rows[i]["STOCK_NO"] = "SYA";			//库号
				bcls_rec->Tables["WM_STOCK"].Rows[i]["STOCK_PLACE_NO"] = "SYA";						//材料库位号
				bcls_rec->Tables["WM_STOCK"].Rows[i]["ROWNO"] = " ";								//行号
				bcls_rec->Tables["WM_STOCK"].Rows[i]["COLUMN_NO"] = " ";							//列号
				bcls_rec->Tables["WM_STOCK"].Rows[i]["LAYERNO"] = 0;								//层号
				bcls_rec->Tables["WM_STOCK"].Rows[i]["STOCK_PLACE_POSITION"] = "1";				//库位内位置
				
			}
			#pragma endregion	
		}

		if (bcls_rec->Tables["MM0099"].Rows.get_Count() > 0)
		{
			doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}


		if (bcls_rec->Tables["MMSMACSH"].Rows.get_Count() > 0)
		{
			doFlag = f_mmsmacsh_snd(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}

		// 调用仓库接口，生成入库队列
		if (bcls_rec->Tables["WM_STOCK"].Rows.get_Count() > 0) {
			Log::Trace("", "", "WM_STOCK", bcls_rec->Tables["WM_STOCK"].Rows.get_Count());
			//doFlag = f_wmsmsm_stock_in(bcls_rec, bcls_ret, conn);  //太钢定制   产出时入库
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
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
