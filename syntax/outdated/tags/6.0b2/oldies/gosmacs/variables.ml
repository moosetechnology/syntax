;;; Functions to save and restore named text.
;;; Written Wed Aug 20 1986 by Philippe Deschamp (PhD) of INRIA.


(defun
    (region-to-variable variable; ^Xx
	(setq variable (concat "Variable " (arg 1 "Variable? ")))
	(save-excursion
	    (temp-use-buffer variable)
	    (setq needs-checkpointing 0))
	(delete-region-to-buffer variable)
	(message variable " set."))

    (variable-to-region variable; ^Xg
	(setq variable (concat "Variable " (arg 1 "Variable? ")))
	(save-excursion
	    (temp-use-buffer variable)
	    (beginning-of-file)
	    (set-mark)
	    (end-of-file)
	    (setq variable (region-to-string)))
	(set-mark)
	(insert-string variable))
)
