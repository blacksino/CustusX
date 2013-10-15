###########################################################
##               Resource Core library
###########################################################

set(QXT_FOLDER ${CustusX3_SOURCE_DIR}/source/ThirdParty/qxt)

cx_add_class_qt_moc(CX_RESOURCE_WIDGETS_FILES
	sscOptimizedUpdateWidget
	sscConsoleWidget
	sscDoubleWidgets
	sscLabeledComboBoxWidget
	sscLabeledLineEditWidget
	sscCheckBoxWidget
	sscHelperWidgets
	sscFileSelectWidget
	sscMousePadWidget
	sscColorSelectWidget
	cxColorSelectButton
	cxDisplayTimerWidget
	cxTimelineWidget
	cxDoubleSpanSlider
	cxBaseWidget
	cxTransform3DWidget
	cxSelectionGroupBox
	cxVector3DWidget
	cxBoundingBoxWidget
	cxFileWatcherWidget
	cxFilePreviewWidget
	cxImagePreviewWidget
	cxPresetWidget
	${QXT_FOLDER}/qxtspanslider_p.h
	${QXT_FOLDER}/qxtspanslider
	)

cx_add_class(CX_RESOURCE_WIDGETS_FILES
	cxTabbedWidget
	snwSyntaxHighlighter
    )

unset(CX_RESOURCE_WIDGETS_INCLUDE_DIRECTORIES)
set(CX_RESOURCE_WIDGETS_INCLUDE_DIRECTORIES
    ${CX_RESOURCE_INCLUDE_DIRECTORIES}
    ${CustusX3_SOURCE_DIR}/source/resource/widgets
    ${QXT_FOLDER}
    CACHE INTERNAL "Include directories for resource widgets library")
include_directories(${CX_RESOURCE_WIDGETS_INCLUDE_DIRECTORIES} )

set(CX_RESOURCE_WIDGETS_LIBRARIES
    ${CX_RESOURCE_WIDGETS_LIBRARIES}
    cxResource
    )

add_library( cxResourceWidgets ${CX_RESOURCE_WIDGETS_FILES})
target_link_libraries(cxResourceWidgets ${CX_RESOURCE_WIDGETS_LIBRARIES} )
CX_COTIRE_ADD_LIBRARY( cxResourceWidgets "cxResource")

#add_subdirectory(testing)