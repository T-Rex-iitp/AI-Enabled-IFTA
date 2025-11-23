object AreaConfirm: TAreaConfirm
  Left = 0
  Top = 0
  Caption = 'Comfirm'
  ClientHeight = 179
  ClientWidth = 279
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  TextHeight = 13
  object OkButton: TButton
    Left = 56
    Top = 120
    Width = 75
    Height = 25
    Caption = 'Ok'
    TabOrder = 0
    OnClick = OkButtonClick
  end
  object CancelButton: TButton
    Left = 137
    Top = 120
    Width = 75
    Height = 25
    Caption = 'Cancel'
    TabOrder = 1
    OnClick = CancelButtonClick
  end
  object AreaName: TEdit
    Left = 80
    Top = 24
    Width = 121
    Height = 21
    TabOrder = 2
    Text = 'Area'
  end
  object ColorBox1: TColorBox
    Left = 80
    Top = 64
    Width = 121
    Height = 22
    Selected = clRed
    Style = [cbStandardColors, cbExtendedColors, cbSystemColors, cbCustomColor]
    TabOrder = 3
  end
end
