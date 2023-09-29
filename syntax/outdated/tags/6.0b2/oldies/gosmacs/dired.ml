;;; Modified Wed Aug 13 16:22:59 1986		;;;
;;; by Philippe Deschamp (PhD) of INRIA		;;;
;;; to fix various problems (see code).		;;;

; This is an attempt at a dired package.

(declare-global Dired-directory)
    (setq Dired-directory (working-directory))

(defun
    (dired directory
	   (save-window-excursion
	       (setq directory (if (> (nargs) 0); PhD
				   (arg 1)
				   (get-tty-file (concat ": dired on directory ["
							 Dired-directory "] ? "))))
	       (if (= directory "")
		   (setq directory Dired-directory))
	       (if (& (= "/" (substr directory -1 1))
		      (> (length directory) 1)); PhD
		   (setq directory (substr directory 1 -1)))
	       (setq Dired-directory directory)
	       (pop-to-buffer "dired")
	       (setq mode-line-format
		     (concat "  Editing  directory:  " directory
			     "      %M   %[%p%]"))
	       (erase-buffer)  (set-mark)
	       (message "Getting contents of " directory); PhD
	       (sit-for 0)
	       (fast-filter-region (concat "ls -la "; PhD
					   (expand-file-name directory)))
	       (beginning-of-file)
	       (if (looking-at "total")
		   (progn
			 (next-line) (next-line) (next-line); PhD
			 (erase-region); PhD
			 (re-replace-string "^." " &"))
		   (progn
			 (end-of-file)
			 (error-message (region-to-string))))
	       (message "Type ^C to exit, ? for help")
	       (save-excursion (recursive-edit))
	       (&dired-done)
	       (message "dired: Come back whenever you want...")
	       (delete-window)
	       (novalue)))
)

(defun
    (&dired-Mark-file-deleted
	(&dired-verify-running); PhD
	(beginning-of-line)
	(if (looking-at " d")
	    (error-message "Can't delete a directory with dired"))
	(delete-next-character)
	(insert-string "D")
	(next-line)
	(beginning-of-line))

    (&dired-summary
	(&dired-verify-running); PhD
	(message "d-elete, u-ndelete, q-uit, r-emove, e,v-isit, n-ext, p-revious, ?-help"))

    (&dired-UnMark-file-deleted
	(&dired-verify-running); PhD
	(beginning-of-line)
	(delete-next-character)
	(insert-string " ")
	(next-line)
	(beginning-of-line))

    (&dired-backup-unmark
	(&dired-verify-running); PhD
	(if (! (bobp))
	    (previous-line))
	(beginning-of-line)
	(delete-next-character)
	(insert-string " ")
	(beginning-of-line))
)

(defun
    (&dired-examine		; PhD: rewritten
	(&dired-verify-running)
	(save-excursion
	    (beginning-of-line)
	    (forward-character)
	    (if (= (following-char) 'l')
		(error-message "Cannot visit a symbolic link!"))
	    (visit-file (&dired-get-fname))
	    (local-bind-to-key "exit-emacs" "\^X\^Q")
	    (message "Type ^X^Q to return to DIRED")
	    (recursive-edit)))
)

(defun
    (&dired-remove
	(&dired-verify-running); PhD
	(beginning-of-line)
	(kill-to-end-of-line) (kill-to-end-of-line))

    (&dired-verify-running; PhD
	(if (= 0 (buffer-size))
	    (error-message "dired already done!")))

    (&dired-get-fname
	(save-excursion
	    (beginning-of-line)
	    (goto-character (+ (dot) 46)); PhD
	    (set-mark)
	    (end-of-line)
	    (concat Dired-directory "/" (region-to-string))))

    (&dired-done		; PhD: rewritten
	(&dired-verify-running)
	(beginning-of-file)
	(error-occurred
	    (re-search-forward "^[^ ]")
	    (beginning-of-line)
	    (if (&dired-get-tty-yes/no "Delete marked files" 1)
		(while (! (eobp))
		       (if (= (following-char) 'D')
			   (progn thisfile
				  (setq thisfile (&dired-get-fname))
				  (if (&dired-get-tty-yes/no (concat "Delete " thisfile) 1)
				      (if (unlink-file thisfile)
					  (progn
						(message "Couldn't delete " thisfile "!")
						(sit-for 2))))))
		       (next-line)))))
)
    
(defun					; PhD
   (&dired-get-tty-yes/no prompt answer default
	(setq default 0)
	(setq prompt (if (> (nargs) 0)
			 (progn (if (> (nargs) 1)
				    (setq default (arg 2)))
				(arg 1))
			 "Yes or No"))
	(setq prompt (concat prompt " [" (if default "Yes" "No") "] ? "))
	(setq answer -1)
	(while (> 0 answer)
	       (setq answer (get-tty-string prompt))
	       (setq answer (if (= answer "")
				default
				(| (= answer "y")
				   (= answer "Y")
				   (= answer "yes")
				   (= answer "Yes")
				   (= answer "YES"))
				1
				(| (= answer "n")
				   (= answer "N")
				   (= answer "no")
				   (= answer "No")
				   (= answer "NO"))
				0
				(progn
				      (save-excursion
					  (pop-to-buffer "Help")
					  (erase-buffer)
					  (insert-string "Please answer Yes or No"))
				      (send-string-to-terminal "\^G")
				      -1))))
	answer)
)

(progn loop
    (save-excursion
	(temp-use-buffer "dired")
	(setq needs-checkpointing 0); PhD
	(define-keymap "&dired-keymap")
	(use-local-map "&dired-keymap")
	(setq loop 0)
	(while (<= loop 127)
	       (local-bind-to-key "&dired-summary" loop)
	       (setq loop (+ loop 1)))
	(setq loop '0')
	(while (<= loop '9')
	       (local-bind-to-key "digit" loop)
	       (setq loop (+ loop 1)))
	(local-bind-to-key "&dired-Mark-file-deleted" "d")
	(local-bind-to-key "&dired-Mark-file-deleted" "D")
	(local-bind-to-key "&dired-Mark-file-deleted" "\^D"); PhD
	(local-bind-to-key "&dired-backup-unmark" "\0177")
	(local-bind-to-key "previous-line" "\^H")
	(local-bind-to-key "previous-line" "p")
	(local-bind-to-key "previous-line" "P")
	(local-bind-to-key "previous-line" "\^P")
	(local-bind-to-key "next-line" "n")
	(local-bind-to-key "next-line" "N")
	(local-bind-to-key "next-line" "\^N")
	(local-bind-to-key "next-line" 13)
	(local-bind-to-key "next-line" 10)
	(local-bind-to-key "next-line" " ")
	(local-bind-to-key "search-forward" "\^S"); PhD
	(local-bind-to-key "search-reverse" "\^R"); PhD
	(local-bind-to-key "argument-prefix" "\^U")
	(local-bind-to-key "previous-window" "\^Xp")
	(local-bind-to-key "previous-window" "\^XP")
	(local-bind-to-key "next-window" "\^Xn")
	(local-bind-to-key "next-window" "\^XN")
	(local-bind-to-key "delete-window" "\^Xd")
	(local-bind-to-key "delete-window" "\^XD")
	(local-bind-to-key "delete-other-windows" "\^X1")
	(local-bind-to-key "&dired-examine" "\^X\^V")
	(local-bind-to-key "next-page" "\^V")
	(local-bind-to-key "previous-page" "\ev")
	(local-bind-to-key "previous-page" "\eV")
	(local-bind-to-key "beginning-of-file" "\e<")
	(local-bind-to-key "end-of-file" "\e>")
	(local-bind-to-key "&dired-UnMark-file-deleted" "u")
	(local-bind-to-key "&dired-UnMark-file-deleted" "U")
	(local-bind-to-key "exit-emacs" "\^C")
	(local-bind-to-key "exit-emacs" "\^X\^Q"); PhD
	(local-bind-to-key "exit-emacs" "q")
	(local-bind-to-key "exit-emacs" "Q")
	(local-bind-to-key "&dired-examine" "e")
	(local-bind-to-key "&dired-examine" "E")
	(local-bind-to-key "&dired-examine" "v")
	(local-bind-to-key "&dired-examine" "V")
	(local-bind-to-key "&dired-remove" "r")
	(local-bind-to-key "&dired-remove" "R"))
)
