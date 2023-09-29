; visit and save remote files via the 4.2 BSD rcp command.
; copyright (c) 1984, Michael Caplinger
; Freely distribute to anyone.

; (rcp-visit-file hostname:filename)
; rcps the file into a file called (net-basename filename)
; in /tmp (so auto-execute will usually work),
; visits it, locally binds ^X^S and ^X^W to the
; remote equivalents, and deletes the temp file.  If the file doesn't
; exist, it complains but leaves the buffer around and rebinds the
; save commands anyway.  The tempfile has to have the name that it does so
; that auto-execute will work on it - an earlier version that did a read-file
; of a more innocuous file name didn't handle auto-execute correctly.
; The mode line of the buffer is massively hacked to make you believe
; the file you're editing is the one on the remote system.

; (rcp-write-file hostname:filename) saves the buffer's contents into a file
; called .rcpf in the current directory, rcps it to filename on the remote 
; system, and deletes the temp file.  rcp-write-named-file asks the user for
; the remote file name; it gives you the default if you just hit return.

; Note that in all cases, if filename isn't an absolute path name, it's
; relative to the user's home directory on the remote machine, as with rcp.

; BUGS: it doesn't act just like the file were local with a funny name.
; Filename completion doesn't exist.  It doesn't make backup copies.
; .emacs_uid files say you're editing the non-existent temp file.
; Two people trying to access a remote file with the same basename at the
; same time are screwed.  Oh well.
; If these bugs concern you (and they should), lobby harder for a real 
; distributed file system at your site.

(declare-buffer-specific *def-filename)

(defun

    (rcp-visit-file filename machine tempname
	(setq filename (arg 1 ": rcp-visit-file "))
	(setq tempname (concat "/tmp/" (net-basename filename)))
	(execute-monitor-command (concat "rcp " filename " " tempname))
	(error-occurred (delete-buffer tempname))
	(if (error-occurred (visit-file tempname))
	    (progn
		; (delete-buffer (net-basename filename))
		(message (concat "Couldn't find " filename))
	    )
	    (unlink-file tempname)
	)
	(setq *def-filename filename)
	(setq mode-line-format (rcp-hack-mode-line mode-line-format filename))
	(rcp-do-local-bindings)
	(novalue)
    )
    
    (rcp-write-file
	(rcp-write-named-file *def-filename)
    )

    (rcp-write-named-file filename
	(write-named-file ".rcpf")
	(setq filename (arg 1
	    (concat ": rcp-write-file [" *def-filename "] ")))
	(if (= filename "")
	    (setq filename *def-filename)
	    (setq *def-filename filename)
	)
	(rcp-do-local-bindings)
	(setq mode-line-format (rcp-hack-mode-line mode-line-format filename))
	(execute-monitor-command (concat "rcp .rcpf " filename))
	(unlink-file ".rcpf")
	(message (concat "Wrote " filename "."))
	(novalue)
    )

    (rcp-do-local-bindings
	(local-bind-to-key "rcp-write-file" "\^X\^S")
	(local-bind-to-key "rcp-write-named-file" "\^X\^W")
    )
)

(defun
    ; net-basename takes a full pathname with slashes and colons and returns
    ; the last element of it; ie, 
    ; "dione:/tmp/foo.t" => "foo.t"
    ; "dione:foo.t" => "foo.t"
    (net-basename
	(save-window-excursion
	    (pop-to-buffer "*rcp-scratch*")
	    (setq needs-checkpointing 0)
	    (erase-buffer)
	    (insert-string (arg 1 ": full pathname "))
	    (if (error-occurred (re-search-reverse "[:/]"))
		(beginning-of-file)
		(forward-character))
	    (set-mark)
	    (end-of-file)
	    (region-to-string)))
    
    (rcp-hack-mode-line
	(save-excursion
	    (temp-use-buffer "*rcp-scratch*")
	    (setq needs-checkpointing 0)
	    (erase-buffer)
	    (insert-string (arg 1))
	    (beginning-of-file)
	    (error-occurred (replace-string "%m" "remote, %m"))
	    (beginning-of-file)
	    (error-occurred (replace-string "%f" (arg 2)))
	    (beginning-of-file)
	    (set-mark)
	    (end-of-file)
	    (region-to-string)))
)
