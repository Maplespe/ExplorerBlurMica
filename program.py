import tkinter
import customtkinter
import subprocess

customtkinter.set_default_color_theme("blue")
clearAddress="false"
clearBarBg="false"
clearWinUIBg= "false"
showLine="false"
mode="0"

class App(customtkinter.CTk):
    def __init__(self):
        super().__init__()

        # configure window
        self.title("ExplorerBlurMica GUI")
        self.geometry(f"{500}x{550}")
        self.resizable(False, False)

        # configure grid layout (4x4)
        self.grid_columnconfigure(1, weight=1)
        self.grid_columnconfigure((2, 3), weight=0)
        self.grid_rowconfigure((0, 1, 2), weight=1)

        # create sidebar frame with widgets
        self.sidebar_frame = customtkinter.CTkFrame(self, width=140, corner_radius=0)
        self.sidebar_frame.grid(row=0, column=0, rowspan=4, sticky="nsew")
        self.sidebar_frame.grid_rowconfigure(4, weight=1)
        self.logo_label = customtkinter.CTkLabel(self.sidebar_frame, text="ExplorerBlurMica", font=customtkinter.CTkFont(size=18, weight="bold"))
        self.logo_label.grid(row=0, column=0, padx=20, pady=(20, 10))
        self.sidebar_button_1 = customtkinter.CTkButton(self.sidebar_frame, command=self.install)
        self.sidebar_button_1.grid(row=1, column=0, padx=20, pady=10)
        self.sidebar_button_2 = customtkinter.CTkButton(self.sidebar_frame, command=self.remove)
        self.sidebar_button_2.grid(row=2, column=0, padx=20, pady=10)
        self.appearance_mode_label = customtkinter.CTkLabel(self.sidebar_frame, text="GUI by lengoc-quang", anchor="w")
        self.appearance_mode_label.grid(row=5, column=0, padx=10, pady=(10, 10))

        # create radiobutton frame
        self.radiobutton_frame = customtkinter.CTkFrame(self)
        self.label_radio_group = customtkinter.CTkLabel(master=self.radiobutton_frame, text="Mode")
        self.label_radio_group.grid(row=0, column=2, columnspan=1, padx=30, pady=(20, 20), sticky="")
        self.radiobutton_frame.grid(row=0, column=2, padx=30, pady=(30, 20), sticky="nsew")
        self.radio_var = tkinter.IntVar(value=0)
        self.radio_button_1 = customtkinter.CTkRadioButton(master=self.radiobutton_frame, variable=self.radio_var, value=0, command=self.changeMode0)
        self.radio_button_1.grid(row=1, column=2, pady=10, padx=20, sticky="n")
        self.radio_button_2 = customtkinter.CTkRadioButton(master=self.radiobutton_frame, variable=self.radio_var, value=1, command=self.changeMode1)
        self.radio_button_2.grid(row=2, column=2, pady=10, padx=20, sticky="n")
        self.radio_button_3 = customtkinter.CTkRadioButton(master=self.radiobutton_frame, variable=self.radio_var, value=2, command=self.changeMode2)
        self.radio_button_3.grid(row=3, column=2, pady=10, padx=20, sticky="n")
        self.radio_button_4 = customtkinter.CTkRadioButton(master=self.radiobutton_frame, variable=self.radio_var, value=3, command=self.changeMode3)
        self.radio_button_4.grid(row=4, column=2, pady=10, padx=20, sticky="n")
        self.radio_button_5 = customtkinter.CTkRadioButton(master=self.radiobutton_frame, variable=self.radio_var, value=4, command=self.changeMode4)
        self.radio_button_5.grid(row=5, column=2, pady=10, padx=20, sticky="n")

        # create checkbox and switch frame
        self.checkbox_slider_frame = customtkinter.CTkFrame(self)
        self.checkbox_slider_frame = customtkinter.CTkLabel(master=self.radiobutton_frame, text="Option")
        self.checkbox_slider_frame.grid(row=0, column=2, columnspan=1, padx=30, pady=(20, 50), sticky="nsew")
        self.checkbox_1 = customtkinter.CTkCheckBox(master=self.checkbox_slider_frame, command=self.clrAddressBar)
        self.checkbox_1.grid(row=1, column=0, pady=(20, 0), padx=(18, 20), sticky="n")     
        self.checkbox_2 = customtkinter.CTkCheckBox(master=self.checkbox_slider_frame, command=self.clrToolbar)
        self.checkbox_2.grid(row=2, column=0, pady=(20, 0), padx=(3, 20), sticky="n")
        self.checkbox_3 = customtkinter.CTkCheckBox(master=self.checkbox_slider_frame, command=self.clrBackground)
        self.checkbox_3.grid(row=3, column=0, pady=(20, 0), padx=20, sticky="n")
        self.checkbox_4 = customtkinter.CTkCheckBox(master=self.checkbox_slider_frame, command=self.showLn)
        self.checkbox_4.grid(row=4, column=0, pady=20, padx=(21, 20), sticky="n")

        # set default values
        self.sidebar_button_1.configure(text="Install")
        self.sidebar_button_2.configure(text="Remove")
        self.checkbox_1.configure(text="Clear address bar ")
        self.checkbox_2.configure(text="Clear toolbar     ")
        self.checkbox_3.configure(text="Clear background  ")
        self.checkbox_4.configure(text="Show seperate line")
        self.radio_button_1.configure(text="Blur          ")
        self.radio_button_2.configure(text="Acrylic       ")
        self.radio_button_3.configure(text="Mica          ")
        self.radio_button_4.configure(text="Clear         ")
        self.radio_button_5.configure(text="Mica Alt      ")
        customtkinter.set_appearance_mode("Light")

    def clrAddressBar(a):
        global clearAddress
        if clearAddress=="false":
            clearAddress="true"
        else:
            clearAddress="false"

    def clrToolbar(a):
        global clearBarBg
        if clearBarBg=="false":
            clearBarBg="true"
        else:
            clearBarBg="false"

    def clrBackground(a):
        global clearWinUIBg
        if clearWinUIBg=="false":
            clearWinUIBg="true"
        else:
            clearWinUIBg="false"

    def showLn(a):
        global showLine
        if showLine=="false":
            showLine="true"
        else:
            showLine="false"
    
    def changeMode0(a):
        global mode
        mode = "0"
    def changeMode1(a):
        global mode
        mode = "1"
    def changeMode2(a):
        global mode
        mode = "2"
    def changeMode3(a):
        global mode
        mode = "3"
    def changeMode4(a):
        global mode
        mode = "4"

    def install(a):
        print(mode, clearAddress, clearBarBg, clearWinUIBg, showLine)
        f = open('config.ini', "w")
        f.write("[config]\n")
        f.write("effect=" + mode + "\n")
        f.write("clearAddress=" + clearAddress + "\n")
        f.write("clearBarBg=" + clearBarBg + "\n")
        f.write("clearWinUIBg=" + clearWinUIBg + "\n")
        f.write("showLine=" + showLine + "\n")
        f.write('''[light]
r=255
g=255
b=255
a=200
[dark]
r=0
g=0
b=0
a=120''')
        f.close()
        subprocess.Popen(".\\register.cmd", shell=True)
        return
    def remove(a):
        subprocess.Popen(".\\uninstall.cmd", shell=True)
        return

if __name__ == "__main__":
    app = App()
    app.mainloop()
