#ifndef pure_relcaseH
#define pure_relcaseH

class ENGINE_API pure_relcase{
private:
	int							m_ID;
public:
	template <typename _class_type>
			pure_relcase		(void (xr_stdcall _class_type::* function_to_bind)(CObject*))
	{
		VERIFY					(g_pGameLevel);
		_class_type *self = static_cast<_class_type *>(this);
		g_pGameLevel->Objects.relcase_register	(
			CObjectList::RELCASE_CALLBACK (
				self,
				function_to_bind
			),
			&m_ID
		);
	}
	virtual ~pure_relcase();
};

#endif //
