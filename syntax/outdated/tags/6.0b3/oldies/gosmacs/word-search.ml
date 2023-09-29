;;; Multi-word searches, by Philippe Deschamp (PhD)	;;;
;;; of INRIA.  Written Thu Aug 14 15:09:20 1986		;;;

(declare-global &search-words-previous)
    (if (! (is-bound &search-words-previous))
	(setq &search-words-previous ""))

(defun
    (search-words-forward
	(re-search-forward (multi-word-arg (arg 1 ": search-words-forward "))))

    (search-words-backward
	(re-search-reverse (multi-word-arg (arg 1 ": search-words-backward "))))

    (Search-Words-Forward fold arg error
	(setq arg (multi-word-arg (arg 1 ": Search-Words-Forward ")))
	(setq fold case-fold-search)
	(setq case-fold-search 1)
	(setq error (error-occurred (re-search-forward arg)))
	(setq case-fold-search fold)
	(if error
	    (error-message "Can't find it.")))

    (Search-Words-Backward fold arg error
	(setq arg (multi-word-arg (arg 1 ": Search-Words-Backward ")))
	(setq fold case-fold-search)
	(setq case-fold-search 1)
	(setq error (error-occurred (re-search-reverse arg)))
	(setq case-fold-search fold)
	(if error
	    (error-message "Can't find it.")))

    (multi-word-arg ro mod chk words
	(setq ro read-only)
	(setq mod buffer-is-modified)
	(setq chk needs-checkpointing)
	(if (= (setq words (arg 1)) "")
	    (setq words &search-words-previous)
	    (error-occurred
		(setq read-only 0)
		(setq buffer-is-modified 1)
		(setq needs-checkpointing 0)
		(save-excursion
		    (save-restriction
			(set-mark)
			(narrow-region)
			(insert-character ' ')
			(insert-string words)
			(insert-character ' ')
			(beginning-of-file)
			(error-occurred (re-replace-string "\\W\\W*" "\\\\b\\\\W*"))
			(forward-character)(forward-character); after \b, before \W*
			(delete-next-character)(delete-next-character)(delete-next-character); \W*
			(end-of-file)	; after \b\W*
			(delete-previous-character)(delete-previous-character); W*
			(backward-character)(backward-character)(backward-character); before \b\
			(setq words (region-to-string))
			(end-of-file)
			(insert-character '|')
			(insert-string words)
			(insert-character '$')
			(setq words (region-to-string))
			(setq &search-words-previous words)
			(erase-region))))
	    (setq &search-words-previous ""))
	(setq read-only ro)
	(setq buffer-is-modified mod)
	(setq needs-checkpointing chk)
	(if (= &search-words-previous "")
	    (error-message "multi-word-arg encountered an error.")
	    words))
)
