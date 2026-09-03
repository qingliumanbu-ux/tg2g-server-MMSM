
/// <summary>
/// 功能说明:消耗物料导入
/// </summary>
#include "stdafx.h"

// Service 入口
BM2F_ENTERACE(mmsm82d_save)
int f_mmsm82d_save(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	int doFlag = 0;
	int affectRows = 0;
	CDecimal seq_id = 0;
	CString sqlstr = " ";
	CString dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");	
	CModel tmmsm56a("TMMSM56A");

	CDbCommand cmd_inq(conn);

	try
	{	

			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				tmmsm56a.Reset();
				tmmsm56a.MergeFrom(bcls_rec->Tables[0].Rows[i]);
				tmmsm56a["STAT_DATE"] = tmmsm56a["OUT_STOCK_TIME"].ToString().SubstringNE(0, 6);
				if (i == 0)
				{
					sqlstr = " delete from  tmmsm56a "
						" where   1=1"
						" and send_flag ! = '1'"
						" and stat_date =@stat_date"
						;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("stat_date", tmmsm56a["STAT_DATE"].ToString());
					cmd_inq.ExecuteNonQuery();
					cmd_inq.Close(); 

					sqlstr = " select max(SEQ_ID) from tmmsm56a"
						" where   1=1"
						" and stat_date =@stat_date"
						;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("stat_date", tmmsm56a["STAT_DATE"].ToString());
					cmd_inq.ExecuteReader();
					if (cmd_inq.Read())
					{
						seq_id = cmd_inq.GetDecimal(1);
					}
					cmd_inq.Close();
				} 
				if (tmmsm56a["FT_FLAG"].ToString() == "3")	 //对应到炉号
				{
					if (tmmsm56a["HEAT_NO"].ToString().Trim() == "")
					{
						strcpy(s.msg, "物料编码" + tmmsm56a["MAT_CODE"].ToString() + "分摊标记为 3，需要录入流通处理号！");
						throw CApplicationException(-1, s.msg, log.Location);
					}
					if (tmmsm56a["DEV_REMARK_1"].ToString().Trim() == "")
					{
						//需要判断是否有该处理号的机组
						sqlstr = " select sum(devo_wt) from tmmsm2a_send"
							" where 1=1"
							" and  SEND_FLAG = '1' and RTN_FLAG !='1' "
							" and HANDLE_DIV != 'F'"
							" and heat_no= @heat_no"
							;
						cmd_inq.SetCommandText(sqlstr);
						cmd_inq.Parameters.Set("heat_no", tmmsm56a["HEAT_NO"].ToString());
						if (cmd_inq.ExecuteScalar().ToInt32() == 0)
						{
							strcpy(s.msg, "物料编码" + tmmsm56a["MAT_CODE"].ToString() + "分摊类型为 3，原消耗为0，需录入机组！");
							throw CApplicationException(-1, s.msg, log.Location);
						}
						cmd_inq.Close();
					}
				}
				if (tmmsm56a["FT_FLAG"].ToString() == "5"&& tmmsm56a["LOT_NO"].ToString().Trim() == "")	 //根据产量分摊，则必须有批次号
				{
					sqlstr = " select count(1) from tmmsm50 where  QUALITY_FLAS='1'"
						" and mat_code = @mat_code"
						;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("mat_code", tmmsm56a["MAT_CODE"].ToString());
					if (cmd_inq.ExecuteScalar().ToInt32() > 0)
					{
						strcpy(s.msg, "物料编码" + tmmsm56a["MAT_CODE"].ToString() + "使用产量分摊，请维护批次号！");
						throw CApplicationException(-1, s.msg, log.Location);
					}
					cmd_inq.Close();

				}


				if (tmmsm56a["FT_FLAG"].ToString() == "6")	 //精准炉次信息
				{
					if ( tmmsm56a["DEV_REMARK_1"].ToString().Trim() == "" )
					{
						strcpy(s.msg, "物料编码" + tmmsm56a["MAT_CODE"].ToString() + "分摊标记为 6，需要录入机组！");
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}

				if (tmmsm56a["FT_FLAG"].ToString() == "7")	 //按区间来，则必须区间炉号必须有值
				{
					if (tmmsm56a["PROC_COUNT"].ToDecimal() == 0)
					{
						strcpy(s.msg, "物料编码" + tmmsm56a["MAT_CODE"].ToString() + "使用区间分摊，需要维护区间版本号！");
						throw CApplicationException(-1, s.msg, log.Location);
					}

					sqlstr = " select count(1) from tmmsm56a2"
						" where   1=1"
						" and proc_count =@proc_count"
						" and stat_date =@stat_date"
						;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("stat_date", tmmsm56a["STAT_DATE"].ToString());
					cmd_inq.Parameters.Set("proc_count", tmmsm56a["PROC_COUNT"].ToDecimal());
					if(cmd_inq.ExecuteScalar().ToInt16()!=1 )					
					{
						strcpy(s.msg, "物料编码" + tmmsm56a["MAT_CODE"].ToString() + "使用区间分摊，未维护区间炉号！");
						throw CApplicationException(-1, s.msg, log.Location);
					}
					cmd_inq.Close();
				}

				if (tmmsm56a["MAT_CODE_T"].ToString().Trim() != ""&&tmmsm56a["LOT_NO"].ToString().Trim() == "")	 //使用推荐物料，必须有批次号
				{
					sqlstr = " select count(1) from tmmsm50 where  QUALITY_FLAS='1'"
						" and mat_code = @mat_code"
						;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("mat_code", tmmsm56a["MAT_CODE"].ToString());
					if (cmd_inq.ExecuteScalar().ToInt32() > 0)
					{
						strcpy(s.msg, "物料编码" + tmmsm56a["MAT_CODE"].ToString() + "使用推荐物料，请维护批次号！");
						throw CApplicationException(-1, s.msg, log.Location);
					}
					cmd_inq.Close();

				}
				if (tmmsm56a["MAT_TYPE"].ToString() == "Y" &&tmmsm56a["LOT_NO"].ToString().Trim() == "")   //使用分类分摊，必须有批次号
				{
					sqlstr = " select count(1) from tmmsm50 where  QUALITY_FLAS='1'"
						" and mat_code = @mat_code"
						;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("mat_code", tmmsm56a["MAT_CODE"].ToString());
					if (cmd_inq.ExecuteScalar().ToInt32() > 0)
					{
						strcpy(s.msg, "物料编码" + tmmsm56a["MAT_CODE"].ToString() + "使用物料类别分摊，请维护批次号！");
						throw CApplicationException(-1, s.msg, log.Location);
					}
					cmd_inq.Close();

				}

				if ((tmmsm56a["MAT_TYPE"].ToString() == "Y" || tmmsm56a["MAT_CODE_T"].ToString().Trim()!= "") && tmmsm56a["OUT_STOCK_WT"].ToDecimal()< 0)
				{
					strcpy(s.msg, "物料编码" + tmmsm56a["MAT_CODE"].ToString() + "物料类别为Y或是重量为负值，不能使用推荐物料！");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmmsm56a["MAT_TYPE"].ToString() == "Y" )
				{
					if (tmmsm56a["MAT_CODE_T"].ToString().Trim() == "")
					{
					
					sqlstr = " select count(1) from tmmsmw4"
						" where mat_code = @mat_code"
						;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("mat_code", tmmsm56a["MAT_CODE"].ToString());
					if (cmd_inq.ExecuteScalar().ToInt32() == 0)
					{ 
						strcpy(s.msg, "物料编码" + tmmsm56a["MAT_CODE"].ToString() + "物料类别为Y，未维护分类！");
						throw CApplicationException(-1, s.msg, log.Location);
					}
					}
					else
					{ 
							sqlstr = " select count(1) from tmmsmw4"
								" where mat_code = @mat_code"
								;
							cmd_inq.SetCommandText(sqlstr);
							cmd_inq.Parameters.Set("mat_code", tmmsm56a["MAT_CODE_T"].ToString());
							if (cmd_inq.ExecuteScalar().ToInt32() == 0)
							{
								strcpy(s.msg, "物料编码" + tmmsm56a["MAT_CODE"].ToString() + "物料类别为Y，使用的推荐物料编码" + tmmsm56a["MAT_CODE_T"].ToString() + "未维护分类！");
								throw CApplicationException(-1, s.msg, log.Location);
							}
					}

				}
				tmmsm56a["SEQ_ID"] = seq_id+i + 1;
					tmmsm56a["REC_CREATOR"] = s.userid;
					tmmsm56a["REC_CREATE_TIME"] = dateNow;
					tmmsm56a.TrimOrBlank();
					tmmsm56a.Insert();
			}		

	}
	catch (CDbException& ex)  //捕获数据库操作异常 
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。", arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.sysmsg, (const char*)ex.GetMsg(), sizeof(s.sysmsg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}


