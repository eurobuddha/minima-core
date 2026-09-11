# Export audit caught ignored historical reports

The first complete export attempt at d69a16b failed its new student guide audit.
Four report snapshots had been placed beneath a directory named build, which the
repository ignore rules excluded from the commit. Local guide checks passed because
the files existed locally. The exported-project check correctly rejected the package.

The snapshots were moved to student-linked-evidence/reports, their lesson links and
manifest updated, and the tracked inventory checked before another package attempt.
The failed package remains a candidate artifact; it is not the final handover.
