# notes
GUI desktop program for managing notes.<br>
The program is written in C++ and uses the Qt library as the GUI.<br>
This is my own implementation of a note management program.<br>
Something like 'Evernote' and other similar programs (but my own).<br>
<b>But it's free and cross-platform</b>.<br>
I ran it on Linux and macOS, I don't know if it works on Windows (but I guess it should).

### Basic properties:
<lu>
<li>Each note is assigned to a category.</li>
<li>The categories are organized in a tree format (left side of the program window).</li>
<li>Each category can contain an unlimited number of subcategories.</li>
<li>Each category can contain an unlimited number of notes (table on the right side of the window).</li>
<li><b>Notes can be moved between categories</b>.</li>
<li>All data is saved in an SQLite database.</li>
</lu>

### Main Window view:
![scr_notes_main.png](images/scr_notes_main.png)

### Edit notes:
Notes can be added to a specific category.<br>
They can be edited and changed.<br>
They can be removed.<br>
And as I mentioned, they can be moved from one category to another.<br>
Through copy/paste operations you can cooperate with external content sources.<br>

The note editor is simple but supports many operations. It has what it takes (at least for me).

![scr_notes_editor.png](images/scr_notes_editor.png)

